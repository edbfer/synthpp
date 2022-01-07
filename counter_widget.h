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

#include "audio_widget.h"

#include <gtkmm/scale.h>

//this is a widget that produces a counting of n bits, from the audio engine clock, at a desired
//rate set by a HScale

class counter_widget : public audio_widget
{

    public:

        counter_widget(int nports);

        void process();
        void process_ui() {};
        void post_creation_callback();

    protected:

        Gtk::Scale freq_scale;

        int nports;
        long ticks = 0;
        float frequency = 100;
        int active_port = 0;

        void freq_scale_value_changed();

};