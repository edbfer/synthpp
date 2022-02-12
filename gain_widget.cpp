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

#include "gain_widget.h"

#include <gtkmm/adjustment.h>

gain_widget::gain_widget(context* program_context) : audio_widget(program_context, 300, 300)
{
    set_label("Gain");
    set_css_style("widget.css", "widget");

    //create input and output port
    input_port = new port("in", port::port_type::INPUT);
    add_port(input_port);
    output_port = new port("out", port::port_type::OUTPUT);
    add_port(output_port);;

    //create the control
    gain_scale.set_digits(0);
    gain_scale.set_orientation(Gtk::Orientation::VERTICAL);
    gain_scale.set_draw_value(true);
    gain_scale.set_size_request(-1, 300);

    Glib::RefPtr<Gtk::Adjustment> adj = gain_scale.get_adjustment();
    adj->set_value(1.0f);
    adj->set_upper(30.0f);
    adj->set_lower(0.0f);
    adj->set_step_increment(0.1f);

    put(gain_scale, 10, 10);

    gain_scale.signal_value_changed().connect(sigc::mem_fun(*this, &gain_widget::gain_scale_value_changed));
}

void gain_widget::process()
{
    float sample = input_port->pop_sample();
    output_port->push_sample(gain * sample);
}

void gain_widget::process_ui()
{}

void gain_widget::post_creation_callback()
{
    Glib::RefPtr<Gtk::Adjustment> adj = gain_scale.get_adjustment();
    adj->set_value(1.0f);

    set_ready(true);
}

void gain_widget::gain_scale_value_changed()
{
    gain = gain_scale.get_value();
}