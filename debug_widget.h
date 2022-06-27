// Copyright (C) 2021 Eduardo Ferreira
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

#include <vector>

//#include "audio_widget.h"
#include "context.h"
#include "audio_widget.h"
#include "port.h"

class debug_widget : public audio_widget
{
    public:

        debug_widget(context* program_context, int x_pos, int y_pos) : audio_widget(program_context, x_pos, y_pos) {};

        void on_creation_callback();
        void post_creation_callback();
        void process();
        void process_ui();

    protected:

        //add a tick counter
        long int ticks = 0;

};