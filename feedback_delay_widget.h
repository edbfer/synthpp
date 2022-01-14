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

//a somewhat more complicated version of the delay, with feedback

#pragma once

#include <boost/circular_buffer.hpp>
#include "audio_widget.h"
#include "port.h"

#include <gtkmm/scale.h>

class feedback_delay_widget : public audio_widget
{
    public:

        feedback_delay_widget();

        void process();
        void process_ui();
        void post_creation_callback();

    protected:

        port* input_port;
        port* output_port;

        float dry_wet = 0.5f;
        float feedback = 0.5f;

        Gtk::Scale n_samples_selector;
        Gtk::Scale dry_wet_selector;
        Gtk::Scale feedback_selector;

        boost::circular_buffer<float> cbuffer;

        void n_samples_selector_value_changed();
        void dry_wet_selector_value_changed();
        void feedback_selector_value_changed();
};