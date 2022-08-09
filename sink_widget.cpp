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

#include <string>
#include "sink_widget.h"

sink_widget::sink_widget(int nports) : audio_widget(300, 300)
{
    this->nports = nports;
}

void sink_widget::on_creation_callback()
{
    set_label("To Audio device");
    set_css_style("widget.css", "widget");

    //create the output ports
    for(int i = 0; i < nports; i++)
    {
        port* p = new port("device_out:" +  std::to_string(i), port::port_type::INPUT);
        add_port(p);
    }

    samples = new float[nports];
}

sink_widget::~sink_widget()
{
    delete[] samples;
}

void sink_widget::process()
{
    int i = 0;
    for(port* p : *get_port_list())
    {
        samples[i++] = p->pop_sample();
    }
}

void sink_widget::process_ui()
{}

void sink_widget::post_creation_callback()
{
    set_ready(true);
}

float* sink_widget::get_samples()
{
    return samples;
}