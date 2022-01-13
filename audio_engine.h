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
#include "logger.h"
#include "source_widget.h"
#include "sink_widget.h"

#include <queue>
#include <mutex>
#include <vector>
#include <tuple>

#include <pthread.h>
#include <portaudio.h>

class audio_engine
{
    public:

        enum thread_messages{
            QUIT,
        };

        //audio_engine();
        audio_engine(std::vector<audio_widget*>* widget_list, std::vector<signal_path*>* path_list, logger& log);

        ~audio_engine();

        std::queue<thread_messages>* get_messaging_queue();
        std::vector<std::pair<int, PaDeviceInfo*>>* get_device_vector();

        void set_current_device(int id);

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
        static int stream_callback(const void* input, void* output, unsigned long frame_count, const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flags, void* userdata);

        logger log;

        //audio related information
        //number of devices
        int num_devices;
        
        //device list
        std::vector<std::pair<int, PaDeviceInfo*>> device_vector;
        int current_device_index;
        PaDeviceInfo* current_device;

        //source widget, where the samples from the sound card will be deposited
        source_widget* source = nullptr;
        //sink, where the samples to be played will be deposited
        sink_widget* sink = nullptr;

        //stream params
        PaStreamParameters input_params;
        PaStreamParameters output_params;
};