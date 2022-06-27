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
#include <gtk/gtkstylecontext.h>
#include <gtk/gtkcssprovider.h>

port::port(std::string label, port_type p_type)
{
    this->direction = p_type;
    this->x = 0;
    this->y = 0;
    this->ui_grabbed = false;
    this->ui_hovered = false;
    this->connected = false;

    base_class = (GtkLabel*) gtk_label_new("");

    //set up font
    GtkCssProvider* style_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(style_provider, "styles/port.css");

    GtkStyleContext* ctx = gtk_widget_get_style_context(GTK_WIDGET(base_class));
    gtk_style_context_add_provider(ctx, GTK_STYLE_PROVIDER(style_provider), GTK_STYLE_PROVIDER_PRIORITY_THEME);
    gtk_widget_add_css_class(GTK_WIDGET(base_class), "port");

    gtk_label_set_text(base_class, label.c_str());
    gtk_label_set_xalign(base_class, 1.0);

    //add touch handler, para que no futuro se faça drag n drop
    gesture_drag = (GtkGestureDrag*) gtk_gesture_drag_new();
    gtk_widget_add_controller(GTK_WIDGET(base_class), GTK_EVENT_CONTROLLER(gesture_drag));
    g_signal_connect(gesture_drag, "drag-begin", G_CALLBACK(mouse_grab_callback), this);

    //create the buffer
    //buffer = boost::circular_buffer<float>(200);
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

void port::mouse_grab_callback(GtkGestureDrag* gdrag, int x, int y, port* port)
{
    //std::cout << x << ", " << y << std::endl;
}

bool port::is_hovered()
{
    return ui_hovered;
}

bool port::is_connected()
{
    return connected;
}

void port::set_connected(bool connected)
{
    this->connected = connected;
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

float port::pop_sample()
{
    /*float res = buffer.front();
    buffer.pop_front();
    return (buffer.size() == 0) ? 0.0f : res;*/
    //float res = sample;
    //sample = 0.f;
    return sample;
}

void port::push_sample(float sample)
{
    //buffer.push_back(sample);
    this->sample = sample;
}

GtkAllocation port::get_allocation()
{
    GtkAllocation alloc;
    gtk_widget_get_allocation(GTK_WIDGET(base_class), &alloc);
    return alloc;
}

std::string port::get_label()
{
    return std::string(gtk_label_get_label(base_class));
}

GtkLabel* port::get_base_class()
{
    return base_class;
}