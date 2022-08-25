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

#include "counter_widget.h"
#include "port.h"

audio_widget* counter_widget::create_instance_4()
{
    return new counter_widget(4);
}
audio_widget* counter_widget::create_instance_8()
{
    return new counter_widget(8);
}
audio_widget* counter_widget::create_instance_16()
{
    return new counter_widget(16);
}

counter_widget::counter_widget(int nports) : audio_widget(), nports(nports)
{
}

void counter_widget::on_creation_callback()
{   
    set_label("Counter: " + std::to_string(nports) + " bits");
    set_css_style("widget.css", "widget");

    // freq_scale.set_digits(0);
    // freq_scale.set_orientation(Gtk::Orientation::VERTICAL);
    // freq_scale.set_draw_value(true);
    // freq_scale.set_size_request(-1, 300);
    // freq_scale.signal_value_changed().connect(sigc::mem_fun(*this, &freq_scale_value_changed));

    // Glib::RefPtr<Gtk::Adjustment> adj = freq_scale.get_adjustment();
    // adj->set_value(100);
    // adj->set_upper(250);
    // adj->set_lower(1);
    // adj->set_step_increment(5);

    // put(freq_scale, 10, 10);
    add_parameter("freq", parameter_types::NUMERIC, 100.f);
    add_control(control_type::scale, "freq_scale", "freq");

    //create as many nports as wanted
    for(int i = 0; i < nports; i++)
    {
        port* p = new port("o" + std::to_string(i), port::port_type::OUTPUT);
        add_port(p);
    }
}

void counter_widget::process()
{
    //we have 5000Hz, -> 5000 ticks per second
    //so 1/5000 seconds per tick, 
    // i % 1 = 0-> 5000Hz
    // i % 2 = 0-> 2500Hz
    // i % 5 = 0-> 1000Hz
    // i % 100 = 0 -> 50Hz
    int step = 5000 / int(std::max(get_numerical_parameter_value("freq"), 1.f));
    ticks++;

    if(ticks % step == 0)
    {
        active_port = (active_port + 1) % nports;
    }

    for(int i = 0; i < nports; i++)
    {
        port* p = get_port_list()->at(i);

        p->push_sample((i == active_port) ? 1. : 0.);
    }
}

void counter_widget::post_creation_callback()
{
    set_ready(true);
}
