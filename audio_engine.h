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

#pragma once

#include "signal_path.h"
#include "audio_widget.h"

#include <queue>
#include <mutex>
#include <vector>

#include <pthread.h>

class audio_engine
{
    public:

        enum thread_messages{
            QUIT,
        };

        //audio_engine();
        audio_engine(std::vector<audio_widget*>* widget_list, std::vector<signal_path*>* path_list);

        ~audio_engine();

        std::queue<thread_messages>& get_messaging_queue();

        void start();
        void process();
        void end();

    protected:

        std::vector<audio_widget*>* widget_list;
        std::vector<signal_path*>* path_list;

        bool is_running;

        //messaging queue
        std::queue<thread_messages> messaging_queue;
        std::mutex queue_mutex;

        //audio thread
        pthread_t audio_engine_thread;

        //main loop
        static void* audio_engine_main_loop(void* instance);

};