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

#include "feedback_delay_widget.h"

audio_widget* feedback_delay_widget::create_instance()
{
    return new feedback_delay_widget();
}

feedback_delay_widget::feedback_delay_widget() : audio_widget()
{
}

void feedback_delay_widget::on_creation_callback()
{
    set_label("Feedback Delay");
    set_css_style("widget.css", "widget");

    //create input and output port
    input_port = new port("in", port::port_type::INPUT);
    add_port(input_port);
    output_port = new port("out", port::port_type::OUTPUT);
    add_port(output_port);

    //create circular buffer
    //at most, we have enough for one second of audio
    cbuffer = boost::circular_buffer<float>(44100, 0.f);

    //create the control
    // n_samples_selector.set_digits(0);
    // n_samples_selector.set_orientation(Gtk::Orientation::VERTICAL);
    // n_samples_selector.set_draw_value(true);
    // n_samples_selector.set_size_request(50, 300);

    add_parameter("nsamp", parameter_types::NUMERIC, 10.f);
    last_size = 10;
    add_control(control_type::scale, "nsamp_scale", "nsamp");
    n_samples_port = new port("nsamp", port::port_type::INPUT);
    add_port(n_samples_port);
    link_port_to_control("nsamplnk", "nsamp", "nsamp_scale");
    
    // Glib::RefPtr<Gtk::Adjustment> adj = n_samples_selector.get_adjustment();
    // adj->set_value(44099);
    // adj->set_upper(44100);
    // adj->set_lower(1);
    // adj->set_step_increment(5);

    // put(n_samples_selector, 10, 10);

    //dry_wet
    // dry_wet_selector.set_digits(1);
    // dry_wet_selector.set_orientation(Gtk::Orientation::VERTICAL);
    // dry_wet_selector.set_draw_value(true);
    // dry_wet_selector.set_size_request(50, 300);

    add_parameter("drywet", parameter_types::NUMERIC, 0.5f);
    add_control(control_type::scale, "drywet_scale", "drywet");
    dry_wet_port = new port("d/w", port::port_type::INPUT);
    add_port(dry_wet_port);
    link_port_to_control("dwlnk", "d/w", "drywet_scale");

    // adj = dry_wet_selector.get_adjustment();
    // adj->set_value(0.5f);
    // adj->set_upper(1.0f);
    // adj->set_lower(0.0f);
    // adj->set_step_increment(0.01f);

    // put(dry_wet_selector, 60, 10);

    //feedback
    // feedback_selector.set_digits(1);
    // feedback_selector.set_orientation(Gtk::Orientation::VERTICAL);
    // feedback_selector.set_draw_value(true);
    // feedback_selector.set_size_request(50, 300);
    add_parameter("fback", parameter_types::NUMERIC, 0.5f);
    add_control(control_type::scale, "fback_scale", "fback");
    feedback_port = new port("fback", port::port_type::INPUT);
    add_port(feedback_port);
    link_port_to_control("fblnk", "fback", "fback_scale");

    // adj = feedback_selector.get_adjustment();
    // adj->set_value(0.5f);
    // adj->set_upper(1.0f);
    // adj->set_lower(0.0f);
    // adj->set_step_increment(0.01f);

    // put(feedback_selector, 110, 10);

    // n_samples_selector.signal_value_changed().connect(sigc::mem_fun(*this, &feedback_delay_widget::n_samples_selector_value_changed));
    // dry_wet_selector.signal_value_changed().connect(sigc::mem_fun(*this, &feedback_delay_widget::dry_wet_selector_value_changed));
    // feedback_selector.signal_value_changed().connect(sigc::mem_fun(*this, &feedback_delay_widget::feedback_selector_value_changed));
}

feedback_delay_widget::~feedback_delay_widget()
{
    delete input_port;
    delete output_port;
    delete n_samples_port;
    delete dry_wet_port;
    delete feedback_port;
}

void feedback_delay_widget::process()
{
    if(get_numerical_parameter_value("nsamp") != last_size)
    {
        cbuffer.rresize(get_numerical_parameter_value("nsamp"), 0.f);
        last_size = get_numerical_parameter_value("nsamp");
    }

    //get value at input port
    float sample = input_port->pop_sample();

    float from_buffer = cbuffer.front();
    cbuffer.pop_front();

    //the resulting sample will be the front of the circular buffer + the current value
    float res = (1.0f - get_numerical_parameter_value("drywet")) * sample + get_numerical_parameter_value("drywet") * from_buffer;

    //push the sample to the back of the buffer
    float to_buffer = (1.0f - get_numerical_parameter_value("fback")) * sample + get_numerical_parameter_value("fback") * res;
    cbuffer.push_back(to_buffer);

    //place the result in the output port
    output_port->push_sample(res);
}

void feedback_delay_widget::process_ui()
{
}

void feedback_delay_widget::post_creation_callback()
{
    set_ready(true);
}