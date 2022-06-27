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
//#include "midi_widget.h"
#include <chrono>
#include <cstring>

audio_engine::audio_engine(std::vector<audio_widget*>* widget_list, std::vector<signal_path*>* path_list, context* program_context)
{
    this->widget_list = widget_list;
    this->path_list = path_list;
    this->is_running = false;
    this->program_context = program_context;

    //create the portaudio context
    PaError err;
    err = Pa_Initialize();
    if(err != paNoError) program_context->log(Pa_GetErrorText(err));

    num_devices = Pa_GetDeviceCount();

    //enumerate devices
    for(int i = 0; i < num_devices; i++)
    {
        PaDeviceInfo* info = (PaDeviceInfo*) Pa_GetDeviceInfo((PaDeviceIndex) i);

        //we want bi-directional devices
        if(info->maxInputChannels > 0 && info->maxOutputChannels > 0)
        {
            program_context->log("Found device " + std::string(info->name) + " with " + std::to_string(info->maxInputChannels) + " input channels, and " + std::to_string(info->maxOutputChannels) + " output channels");
            device_vector.push_back(std::pair(i, info));
        }
    }

    //now its time to create the midi context
    PmError midi_err;
    midi_err = Pm_Initialize();
    if(err != pmNoError) program_context->log(Pm_GetErrorText(midi_err));
    
    //device enumeration
    //need porttime for portmidi timekeeping
    Pt_Start(1, NULL, NULL);

    //enumerate devices
    int cnt = Pm_CountDevices();
    int iter = 0;
    for(int i = 0; i < cnt; i++)
    {
        PmDeviceInfo* info = (PmDeviceInfo*) Pm_GetDeviceInfo(i);
        //std::cout << "Detected: " << info->name << " " << ((info->input) ? "I" : "i") << "/" << ((info->output) ? "O" : "o") << std::endl;
        if(info->input) //is input
        {
            midi_devices_list.push_back(std::make_tuple((PmDeviceID) i, info, (PortMidiStream*) nullptr));
        }
    }

    //add our functions to the context object
    program_context->get_midi_active_devices = sigc::mem_fun(*this, &get_midi_active_devices);
    program_context->register_midi_consumer = sigc::mem_fun(*this, &register_midi_consumer);
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
        program_context->remove_widget(source);
        delete source;
    }
    if (sink != nullptr)
    {
        program_context->remove_widget(sink);
        delete sink;
    }
    source = new source_widget(program_context, current_device->maxInputChannels);
    sink = new sink_widget(program_context, current_device->maxOutputChannels);

    program_context->put_widget(source);
    program_context->put_widget(sink);
}

void audio_engine::start()
{
    if (current_device == nullptr)
    {
        program_context->log("Failure starting audio engine: need to select an audio device");
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

        //populate midi queues
        for(auto midi_device : context->midi_devices_list)
        {
            //is the device active?
            PortMidiStream* midi_stream = std::get<2>(midi_device);

            if(midi_stream != nullptr)
            {
                PmDeviceID dev_id = std::get<0>(midi_device);
                
                populate_midi_queue(context, dev_id, midi_stream); 

                //does anyone want the messages?
                //do we even have messages?
                if(!context->get_midi_device_queue(dev_id)->empty())
                {
                    PmEvent next = context->get_midi_device_queue(dev_id)->front();
                    context->get_midi_device_queue(dev_id)->pop();

                    midi_types type = (midi_types) midi_type(Pm_MessageStatus(next.message));

                    /*for(auto consumer : context->midi_consumers_list)
                    {
                        if(consumer->event_filter(dev_id, type, Pm_MessageData1(next.message)))
                        {
                            //this one wants it
                            consumer->push_midi_event(next);
                        }
                    }*/
                }
            }
        }

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

void audio_engine::populate_midi_queue(audio_engine* engine, PmDeviceID dev_id, PortMidiStream* midi_stream)
{
    PmEvent incoming_events[32]; //we defined max 32
    //are there messages waiting?
    if(Pm_Poll(midi_stream))
    {
        //how many
        int count = Pm_Read(midi_stream, incoming_events, 32);
        for(int i = 0; i < count; i++)
        {
            //push to queue
            //is CC
            int status = Pm_MessageStatus(incoming_events[i].message);
            
            //is CC, is Channel 0
            int type = midi_type(status);
            int channel = midi_channel(status);

            if(type == midi_types::CONTROL_CHANGE && channel == 0)
                engine->get_midi_device_queue(dev_id)->push(incoming_events[i]);
        }
    }
}

void audio_engine::midi_device_set_enabled(PmDeviceID device_id, bool enabled)
{
    for(auto& device : midi_devices_list)
    {
        //find the device and check if not enabled
        if(std::get<0>(device) == device_id)
        {
            if (enabled && std::get<2>(device) == nullptr)
            {
                PortMidiStream* midi_stream;
                Pm_OpenInput(&midi_stream, device_id, NULL, 32, NULL, NULL);

                //create the queue, of 64 events max.
                midi_message_queue[device_id] = std::make_shared<std::queue<PmEvent>>();
                std::get<2>(device) = midi_stream;
                break;
            }
            else if(!enabled && std::get<2>(device) != nullptr)
            {
                Pm_Close(std::get<2>(device));

                std::get<2>(device) = nullptr;
            }
        }
    }    
}

bool audio_engine::is_midi_device_enabled(PmDeviceID device_id)
{
    for(auto device : midi_devices_list)
    {
        if(std::get<0>(device) == device_id)
        {
            return (std::get<2>(device) != nullptr) ? true : false;
        }
    }
    return false;
}

std::vector<std::pair<PmDeviceID, PmDeviceInfo*>> audio_engine::get_midi_device_vector()
{
    std::vector<std::pair<PmDeviceID, PmDeviceInfo*>> temp;

    for(auto device : midi_devices_list)
    {
        temp.push_back(std::pair(std::get<0>(device), std::get<1>(device)));
    }

    return temp;
}

std::vector<std::pair<PmDeviceID, PmDeviceInfo*>> audio_engine::get_midi_active_devices()
{
    std::vector<std::pair<PmDeviceID, PmDeviceInfo*>> active;
    for(auto devices : midi_devices_list)
    {
        if(std::get<2>(devices) != nullptr)
        {
            active.push_back(std::pair(std::get<0>(devices), std::get<1>(devices)));
        }
    }
    return active;
}

std::shared_ptr<std::queue<PmEvent>> audio_engine::get_midi_device_queue(PmDeviceID device_id)
{
    return midi_message_queue.at(device_id);
}

PmEvent audio_engine::get_midi_device_next(PmDeviceID device_id)
{
    if(!midi_message_queue.at(device_id)->empty())
    {
        PmEvent temp = midi_message_queue.at(device_id)->front();
        midi_message_queue.at(device_id)->pop();
        return temp;
    }
    return {-1, -1};
}

void audio_engine::register_midi_consumer(midi_widget* consumer)
{
    midi_consumers_list.push_back(consumer);
}