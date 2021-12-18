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

#include "port.h"

#include <iostream>
#include <gtkmm/cssprovider.h>

port::port(std::string label, port_type p_type)
{
    this->direction = p_type;
    this->x = 0;
    this->y = 0;
    this->ui_grabbed = false;
    this->ui_hovered = false;


    //set up font
    Glib::RefPtr<Gtk::CssProvider> style_provider = Gtk::CssProvider::create();
    style_provider->load_from_path("styles/port.css");
    get_style_context()->add_provider(style_provider, GTK_STYLE_PROVIDER_PRIORITY_THEME);
    add_css_class("port");

    set_text(label);
    set_xalign(1.0);

    //add touch handler, para que no futuro se faça drag n drop
    gesture_drag = Gtk::GestureDrag::create();
    add_controller(gesture_drag);
    gesture_drag->signal_drag_begin().connect(sigc::mem_fun(*this, &port::mouse_grab_callback), false);
}


port::port_type port::get_direction()
{
    return direction;
}

void port::set_position_inwidget(int x, int y)
{
    this->x = x;
    this->y = y;
}

void port::set_position_indarea(int x, int y)
{
    this->darea_x = x;
    this->darea_y = y;
}

void port::get_position_inwidget(int& x, int&y)
{
    x = this->x;
    y = this->y;
}

void port::get_position_indarea(int& x, int&y)
{
    x = this->darea_x;
    y = this->darea_y;
}

void port::mouse_grab_callback(int x, int y)
{
    std::cout << x << ", " << y << std::endl;
}

bool port::is_hovered()
{
    return ui_hovered;
}

void port::set_hovered(bool hover)
{
    ui_hovered = hover;
}

bool port::is_grabbed()
{
    return ui_grabbed;
}

void port::set_grabbed(bool grab)
{
    ui_grabbed = grab;
}