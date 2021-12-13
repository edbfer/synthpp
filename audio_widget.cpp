// Copyright (C) 2021 eduardof
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

#include <cassert>
#include <cmath>

#include <gtkmm/fixed.h>
#include <gtkmm/frame.h>
#include <gtkmm/grid.h>

#include "mainwindow.h"

#include "audio_widget.h"

audio_widget::audio_widget()
{
    set_label("Empty widget");
    set_size_request(200, 200);

    //event
    gesture_drag = Gtk::GestureDrag::create();
    add_controller(gesture_drag);

    //pressed event
    gesture_drag->signal_drag_begin().connect(sigc::mem_fun(*this, &audio_widget::mouse_grab_callback));
    gesture_drag->signal_drag_update().connect(sigc::mem_fun(*this, &audio_widget::mouse_grab_update_callback));  

    //canvas
    set_child(fixed_canvas);
    fixed_canvas.set_expand(true);
    fixed_canvas.set_halign(Gtk::Align::FILL);
    fixed_canvas.set_valign(Gtk::Align::FILL);

    show();
}

void audio_widget::mouse_grab_callback(int x, int y)
{
    //std::cout << "Pressed at (" << x << ", " << y << ")" << std::endl;
}

void audio_widget::mouse_grab_update_callback(int offset_x, int offset_y)
{
    double x = 500., y = 500.;

    //set_label("Moved!");
    
    Gtk::Fixed* parent = (Gtk::Fixed*) this->get_parent();
    parent->get_child_position(*this, x, y);

    //parent limits
    double parent_max_x, parent_max_y;
    parent_max_x = parent->get_allocation().get_width();
    parent_max_y = parent->get_allocation().get_height();

    x = std::max(std::min(x + offset_x, parent_max_x), 0.);
    y = std::max(std::min(y + offset_y, parent_max_y), 0.);

    parent->move(*this, x, y);
    //std::cout << "curpos: " << x << ", " << y << std::endl;
}

audio_widget::~audio_widget()
{
}

void audio_widget::add_port(port* p)
{
    port_vector.push_back(p);

    //place on the drawing
    if(p->get_direction() == port::port_type::OUTPUT)
    {
        //get number of ports
        int idx_port = port_vector.size() - 1;

        int position_x, position_y;
        get_size_request(position_x, position_y);

        position_x -= 10;
        position_y = 10 + idx_port*20;

        fixed_canvas.put(*p, position_x, position_y);
        p->set_position_inwidget(position_x, position_y);
    } 
    else if(p->get_direction() == port::port_type::INPUT)
    {

    }
}

std::vector<port*>* audio_widget::get_port_list()
{
    return &port_vector;
}

void audio_widget::get_underlaying_fixed_position(int& x, int& y)
{
    x = fixed_canvas.get_allocation().get_x();
    y = fixed_canvas.get_allocation().get_y();
}