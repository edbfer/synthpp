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

#include "probe_widget.h"
#include "port.h"

#include <sstream>
#include <iomanip>

probe_widget::probe_widget(context* program_context) :
    audio_widget(program_context, 300, 300)
{
    set_label(std::to_string(0.0f));
    set_css_style("widget.css", "widget");

    set_size_request(100, 50);

    p = new port("input", port::port_type::INPUT);
    add_port(p);
}

void probe_widget::process()
{
    state = p->pop_sample();
}

void probe_widget::process_ui()
{
    std::stringstream output_value;
    output_value << std::setprecision(2) << state;
    set_label(output_value.str());
}

void probe_widget::post_creation_callback()
{
    set_ready(true);
}