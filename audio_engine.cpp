// Copyright (C) 2022 Eduardo Ferreira
// 
// This file is part of synthpp.
// 
// synthpp is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// synthpp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with synthpp.  If not, see <http://www.gnu.org/licenses/>.

#include "audio_engine.h"
#include <chrono>
#include <cstring>

audio_engine::audio_engine(std::vector<audio_widget*>* widget_list, std::vector<signal_path*>* path_list, logger& log)
{
    this->widget_list = widget_list;
    this->path_list = path_list;
    this->is_running = false;
    this->log = log;

    //create the portaudio context
    PaError err;
    err = Pa_Initialize();
    if(err != paNoError) log.log(Pa_GetErrorText(err));

    num_devices = Pa_GetDeviceCount();

    //enumerate devices
    for(int i = 0; i < num_devices; i++)
    {
        PaDeviceInfo* info = (PaDeviceInfo*) Pa_GetDeviceInfo((PaDeviceIndex) i);

        //we want bi-directional devices
        if(info->maxInputChannels > 0 && info->maxOutputChannels > 0)
        {
            log.log("Found device " + std::string(info->name) + " with " + std::to_string(info->maxInputChannels) + " input channels, and " + std::to_string(info->maxOutputChannels) + " output channels");
            device_vector.push_back(std::pair(i, info));
        }
    }
}

void audio_engine::set_current_device(int id)
{
    //id is an index in the vector
    std::pair p = device_vector[id];

    //PaDeviceInfo* devinfo = (PaDeviceInfo*) Pa_GetDeviceInfo((PaDeviceIndex) p.first);
    current_device_index = p.first;
    current_device = p.second;

    //create the input and output widgets
    //input
    if (source != nullptr) 
    {
        log.remove_widget(source);
        delete source;
    }
    if (sink != nullptr)
    {
        log.remove_widget(sink);
        delete sink;
    }
    source = new source_widget(current_device->maxInputChannels);
    sink = new sink_widget(current_device->maxOutputChannels);

    log.put_widget(source);
    log.put_widget(sink);
}

void audio_engine::start()
{
    if (current_device == nullptr)
    {
        log.log("Failure starting audio engine: need to select an audio device");
        return;
    }
    //create the thread;
    pthread_create(&audio_engine_thread, nullptr, &audio_engine::audio_engine_main_loop, this);
}

void audio_engine::end()
{
    //std::lock_guard<std::mutex> lock_queue(queue_mutex);
    if(is_running)
    {
        std::lock_guard<std::mutex> lock_queue(queue_mutex);
        messaging_queue.push(thread_messages::QUIT);
    }
}

audio_engine::~audio_engine()
{
    end();
}

void* audio_engine::audio_engine_main_loop(void* instance)
{
    //parse the argument
    audio_engine* context = (audio_engine*) instance;

    //create the audio context
    std::memset(&(context->input_params), 0, sizeof(PaStreamParameters));
    std::memset(&(context->output_params), 0, sizeof(PaStreamParameters));

    context->input_params.device = context->current_device_index;
    context->input_params.channelCount = context->current_device->maxInputChannels;
    context->input_params.sampleFormat = paFloat32;

    context->output_params.device = context->current_device_index;
    context->output_params.channelCount = context->current_device->maxOutputChannels;
    context->output_params.sampleFormat = paFloat32;

    //open the stream
    PaStream* handle;
    Pa_OpenStream(&handle, &(context->input_params), &(context->output_params), 44100., 32, paNoFlag, &stream_callback, context);

    Pa_StartStream(handle);

    //create the clock
    std::chrono::high_resolution_clock::time_point t1, t2;
    t1 = std::chrono::high_resolution_clock::now();

    context->is_running = true;

    while(true)
    {
        /*//get the current time
        t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(t2 - t1);

        //5000Hz
        if(elapsed.count() >= 1.0f/2756.25f)
        {
            //do cables propagation
            for(signal_path* sp : *(context->path_list))
            {
                sp->propagate(16);
            }

            //process widgets
            for(audio_widget* aw : *(context->widget_list))
            {
                if(aw->is_ready())
                    aw->process();
            }*/

            //check messages
            //{
                std::lock_guard<std::mutex> lock_queue(context->queue_mutex);
                if(context->get_messaging_queue()->size() > 0)
                {
                    thread_messages message = context->get_messaging_queue()->front();
                    context->get_messaging_queue()->pop();

                    if(message == thread_messages::QUIT)
                        break;
                }
            //}

            //t1 = std::chrono::high_resolution_clock::now();
        //}
    }

    Pa_StopStream(handle);
    Pa_CloseStream(handle);
    pthread_exit(nullptr);
    context->is_running = false;
}

std::queue<audio_engine::thread_messages>* audio_engine::get_messaging_queue()
{
    return &messaging_queue;
}

std::vector<std::pair<int, PaDeviceInfo*>>* audio_engine::get_device_vector()
{
    return &device_vector;
}

int audio_engine::stream_callback(const void* input, void* output, unsigned long frame_count, const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flags, void* userdata)
{
    auto t1 = std::chrono::high_resolution_clock::now();

    audio_engine* context = (audio_engine*) userdata;

    int n_input = context->current_device->maxInputChannels;
    int n_output = context->current_device->maxOutputChannels;

    float* input_buffer = (float*) input;
    float* output_buffer = (float*) output;

    for(int i = 0; i < frame_count; i++)
    {
        //each frame will have a value for each of the channels, which correspond to ports in the source/sink widget
        source_widget* source = context->source;

        int j = 0;
        for(port* p : *(source->get_port_list()))
        {
            p->push_sample(input_buffer[i * n_input + j++]);
        }
    
        //processing -> propagate values to the input ports
        for(signal_path* sp : *(context->path_list))
        {
            sp->propagate_input();
        }

        //process widgets
        for(audio_widget* aw : *(context->widget_list))
        {
            if(aw->is_ready())
                aw->process();
        }

        //propagate from the output
        for(signal_path* sp : *(context->path_list))
        {
            sp->propagate_output();
        }
        
        //now is the same thing again
        //write to buffer
        sink_widget* sink = context->sink;

        j = 0;
        for(port* p : *(sink->get_port_list()))
        {
            float sample = sink->get_samples()[j];
            output_buffer[i * n_output + j++] = std::max(std::min(p->pop_sample(), 1.0f), -1.0f);
        }
    }

    // auto t2 = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<float> elapsed = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(t2 - t1);

    // std::cout << "elapsed: " << elapsed.count() << std::endl;

    return paContinue;
}