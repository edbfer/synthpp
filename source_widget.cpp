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
#include "source_widget.h"

source_widget::source_widget(context* program_context, int nports) : audio_widget(program_context, 300, 300)
{
    this->nports = nports;
}

void source_widget::on_creation_callback()
{
    set_label("From Audio device");
    set_css_style("widget.css", "widget");

    //create the output ports
    for(int i = 0; i < nports; i++)
    {
        port* p = new port("device_in:" +  std::to_string(i), port::port_type::OUTPUT);
        add_port(p);
    }
}

void source_widget::process()
{}

void source_widget::process_ui()
{}

void source_widget::post_creation_callback()
{}