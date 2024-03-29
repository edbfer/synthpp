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

#include "delay_widget.h"

audio_widget* delay_widget::create_instance()
{
    return new delay_widget();
}

delay_widget::delay_widget() : audio_widget(300, 300)
{
}

void delay_widget::on_creation_callback()
{
    set_label("Slap-Back");
    set_css_style("widget.css", "widget");

    //create input and output port
    input_port = new port("in", port::port_type::INPUT);
    add_port(input_port);
    output_port = new port("out", port::port_type::OUTPUT);
    add_port(output_port);

    //create circular buffer
    //at most, we have enough for one second of audio
    last_size = 10;
    cbuffer = boost::circular_buffer<float>(10, 0.f);

    // //create the control
    // n_samples_picker.set_digits(0);
    // n_samples_picker.set_orientation(Gtk::Orientation::VERTICAL);
    // n_samples_picker.set_draw_value(true);
    // n_samples_picker.set_size_request(-1, 300);

    // Glib::RefPtr<Gtk::Adjustment> adj = n_samples_picker.get_adjustment();
    // adj->set_value(44099);
    // adj->set_upper(44100);
    // adj->set_lower(1);
    // adj->set_step_increment(5);

    // put(n_samples_picker, 10, 10);

    // n_samples_picker.signal_value_changed().connect(sigc::mem_fun(*this, &delay_widget::n_samples_picker_value_changed));
    add_parameter("nsamp", parameter_types::NUMERIC, 10.f);
    add_control(control_type::scale, "nsamp_scale", "nsamp");
}

void delay_widget::process()
{
    int new_size = get_numerical_parameter_value("nsamp");
    if(new_size != last_size)
    {
        cbuffer.rresize(new_size, 0.f);
        last_size = new_size;
    }

    //get value at input port
    float sample = input_port->pop_sample();

    float from_buffer = cbuffer.front();
    cbuffer.pop_front();

    //the resulting sample will be the front of the circular buffer + the current value
    float res = 0.5f * sample + 0.5f * from_buffer;

    //push the sample to the back of the buffer
    cbuffer.push_back(sample);

    //place the result in the output port
    output_port->push_sample(res);
}

void delay_widget::process_ui()
{}

void delay_widget::post_creation_callback()
{
    set_ready(true);
}