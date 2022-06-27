// Copyright (C) 2021 Eduardo Ferreira
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

#include "signal_path.h"
#include "utils.h"

signal_path::signal_path(port* source, port* dest)
{
    this->source_port = source;
    this->destination_port = dest;

    std::string name = utils::gen_8char_id();
    this->path_name = name;

    this->ui_selected = false;
}

signal_path::~signal_path()
{
    if(source_port != nullptr) source_port->set_connected(false);
    if(destination_port != nullptr) destination_port->set_connected(false);
}

port* signal_path::get_source_port()
{
    return source_port;
}

port* signal_path::get_destination_port()
{
    return destination_port;
}

std::string signal_path::get_path_name()
{
    return path_name;
}

void signal_path::set_source_port(port* source)
{
    //check if there is a port previously
    if(source_port != nullptr)
    {
        source_port->set_connected(false);
    }

    source->set_connected(true);
    this->source_port = source;
}

void signal_path::set_destination_port(port* dest)
{
    //check if there is a port previously
    if(destination_port != nullptr)
    {
        destination_port->set_connected(false);
    }

    dest->set_connected(true);
    this->destination_port = dest;
}

/*void signal_path::propagate(int nsamples)
{
    destination_port->push_sample(source_port->pop_sample());
}*/

void signal_path::propagate_input()
{
    destination_port->push_sample(transport);
}

void signal_path::propagate_output()
{
    transport = source_port->pop_sample();
}

bool signal_path::bounds_check(float mx, float my, float delta)
{
    const int peg_radius = 6;
    const int bias = 2;

    //check if the mouse is within the region
    //get the vector of the wire
    int p2_x, p2_y; destination_port->get_position_indarea(p2_x, p2_y);
    p2_x -= peg_radius;
    p2_y += peg_radius + bias;

    int p1_x, p1_y; source_port->get_position_indarea(p1_x, p1_y);
    p1_x += peg_radius;
    p1_y += peg_radius + bias;

    //wire vec
    float wire_x = p2_x - p1_x;
    float wire_y = p2_y - p1_y;

    //tomouse vec
    float tomouse_x = mx - p2_x;
    float tomouse_y = my - p2_y;

    //cosine between two vectors
    float cosine = (wire_x*tomouse_x + wire_y*tomouse_y) / (hypot(wire_x, wire_y)*hypot(tomouse_x, tomouse_y));
    float sine = sqrt(1.0f - cosine*cosine);

    //distance to the line
    float dist = hypot(tomouse_x, tomouse_y) * sine;

    if(dist <= delta)
        return true;
    else
        return false;
}

bool signal_path::is_ui_selected()
{
    return ui_selected;
}

void signal_path::set_ui_selected(bool ui_selected)
{
    this->ui_selected = ui_selected;
}