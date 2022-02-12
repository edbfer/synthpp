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
#include <pangomm/layout.h>

#include "mainwindow.h"

#include "audio_widget.h"

audio_widget::audio_widget(context* program_context, int x_pos, int y_pos)
{
    this->x_pos = x_pos;
    this->y_pos = y_pos;
    this->program_context = program_context;

    set_label("Empty widget");
    set_size_request(200, 200);

    put(label, 0, 0);

    //event
    gesture_drag = Gtk::GestureDrag::create();
    add_controller(gesture_drag);

    //pressed event
    gesture_drag->signal_drag_begin().connect(sigc::mem_fun(*this, &audio_widget::mouse_grab_callback));
    gesture_drag->signal_drag_update().connect(sigc::mem_fun(*this, &audio_widget::mouse_grab_update_callback));  

    ////add handler to mapped fixed_canvas
    signal_map().connect(sigc::mem_fun(*this, &audio_widget::post_creation_callback));

    //create css provider
    css_provider = Gtk::CssProvider::create();
    get_style_context()->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_THEME);

    isReady = false;
}

void audio_widget::mouse_grab_callback(int x, int y)
{
    //std::cout << "Pressed at (" << x << ", " << y << ")" << std::endl;
    //we only move on the top!
    if (y > 50)
        ignore_mouse_drag = 1;
    else
        ignore_mouse_drag = 0;
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

    this->x_pos = x;
    this->y_pos = y;

    if(!ignore_mouse_drag)
    {
        parent->move(*this, x, y);

        //update the position of the ports in the drawing ara
        for(port* p : port_vector)
        {
            //get the position of the port in the widget
            int port_x, port_y;
            p->get_position_inwidget(port_x, port_y);

            //if we are input, we are actually already on the border, if not we need the width of the label
            int wid = (p->get_direction() == port::port_type::OUTPUT)? p->get_allocation().get_width() : 0;

            p->set_position_indarea(port_x + x + wid, port_y + y);
        }    

        //update the drawing area
        //((MainWindow*) ((Gtk::Grid*) parent->get_parent())->get_parent())->playfield_trigger_redraw();
    }
    //std::cout << "curpos: " << x << ", " << y << std::endl;
    
}

audio_widget::~audio_widget()
{
    for(port* p: this->port_vector)
        delete p;
}

void audio_widget::add_port(port* p)
{
    port_vector.push_back(p);

    //get the position of the widget in the darea
    int wid_x = this->x_pos;
    int wid_y = this->y_pos;

    //lets wait for p to be realized:
    //p->signal_show().connect(sigc::bind(sigc::mem_fun(*this, &audio_widget::on_port_realize), p));

    //need the size
    p->create_pango_layout(p->get_text());

    //place on the drawing
    if(p->get_direction() == port::port_type::OUTPUT)
    {
        //get number of ports
        n_out_ports++;

        int position_x, position_y;
        get_size_request(position_x, position_y);

        int wid, hei;
        p->get_layout()->get_size(wid, hei);

        wid /= PANGO_SCALE;
        hei /= PANGO_SCALE;

        position_x -= wid;
        position_y = 10 + n_out_ports*20;

        put(*p, position_x, position_y);

        p->set_position_inwidget(position_x, position_y);

        //in drawing area
        p->set_position_indarea(position_x + wid_x + wid, position_y + wid_y);
    } 
    else if(p->get_direction() == port::port_type::INPUT)
    {
        //get number of ports
        n_in_ports++;

        int position_x, position_y;
        get_size_request(position_x, position_y);

        position_x = 0;
        position_y = 10 + n_in_ports*20;

        put(*p, position_x, position_y);
        p->set_position_inwidget(position_x, position_y);

        //in drawing area
        p->set_position_indarea(position_x + wid_x, position_y + wid_y);
    }
}

std::vector<port*>* audio_widget::get_port_list()
{
    return &port_vector;
}

void audio_widget::get_underlaying_fixed_position(int& x, int& y)
{
    x = get_allocation().get_x();
    y = get_allocation().get_y();
}

void audio_widget::on_port_realize(port* p)
{
    Gtk::Fixed* fixed_parent = (Gtk::Fixed*) p->get_parent();

    //int fixed_x = fixed_canvas.get_allocation().get_x();
    //int fixed_y = fixed_canvas.get_allocation().get_y();
}

void audio_widget::set_label(std::string label)
{
    this->label.set_label(label);
}

void audio_widget::set_css_style(std::string filename, std::string css_class)
{
    css_provider->load_from_path("styles/" + filename);
    add_css_class(css_class);
}

void audio_widget::set_ready(bool ready)
{
    isReady = ready;
}

bool audio_widget::is_ready()
{
    return isReady;
}