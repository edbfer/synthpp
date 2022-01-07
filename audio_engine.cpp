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


audio_engine::audio_engine(std::vector<audio_widget*>* widget_list, std::vector<signal_path*>* path_list)
{
    this->widget_list = widget_list;
    this->path_list = path_list;
    this->is_running = false;
}

void audio_engine::start()
{
    //create the thread;
    pthread_create(&audio_engine_thread, nullptr, &audio_engine::audio_engine_main_loop, this);
}

void audio_engine::end()
{
    //std::lock_guard<std::mutex> lock_queue(queue_mutex);
    if(is_running)
        messaging_queue.push(thread_messages::QUIT);
}

audio_engine::~audio_engine()
{
    end();
}

void* audio_engine::audio_engine_main_loop(void* instance)
{
    //parse the argument
    audio_engine* context = (audio_engine*) instance;

    //create the clock
    std::chrono::high_resolution_clock::time_point t1, t2;
    t1 = std::chrono::high_resolution_clock::now();

    context->is_running = true;

    while(true)
    {
        //get the current time
        t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(t2 - t1);

        //5000Hz
        if(elapsed.count() >= 1.0f/5000.f)
        {
            //do cables propagation
            for(signal_path* sp : *(context->path_list))
            {
                sp->propagate(1);
            }

            //process widgets
            for(audio_widget* aw : *(context->widget_list))
            {
                if(aw->is_ready())
                    aw->process();
            }

            //check messages
            if(context->get_messaging_queue().size() > 0)
            {
                thread_messages message = context->get_messaging_queue().front();
                context->get_messaging_queue().pop();

                if(message == thread_messages::QUIT)
                    break;
            }

            t1 = std::chrono::high_resolution_clock::now();
        }
    }

    context->is_running = false;
}

std::queue<audio_engine::thread_messages>& audio_engine::get_messaging_queue()
{
    return messaging_queue;
}