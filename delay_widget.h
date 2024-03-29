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

#include "context.h"
#include "audio_widget.h"
#include "port.h"

#include <boost/circular_buffer.hpp>

//this simple plugin implements a simple slap-back echo of the selected number of samples

class delay_widget : public audio_widget
{
    public:
        delay_widget();

        void on_creation_callback();

        void process();
        void process_ui();
        void post_creation_callback();

        static audio_widget* create_instance();

    protected:
        boost::circular_buffer<float> cbuffer;

        int last_size;

        //input, output port
        port* input_port, *output_port;

};