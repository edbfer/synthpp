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

//simple gain widget with one control

#pragma once

#include <gtkmm/scale.h>
#include "audio_widget.h"
#include "port.h"

class gain_widget : public audio_widget
{
    public:
        gain_widget();

        void process();
        void process_ui();
        void post_creation_callback();

    protected:

        float gain;
        port* input_port;
        port* output_port;
        
        void gain_scale_value_changed();
        Gtk::Scale gain_scale;
};