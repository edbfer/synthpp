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

#pragma once

#include "port.h"

#include <iostream>
#include <string>

#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/fixed.h>
#include <gtkmm/gesturedrag.h>
#include <gtkmm/cssprovider.h>

class audio_widget : public Gtk::Fixed
{

    public:

        audio_widget(int x_pos = 300, int y_pos = 300);
        ~audio_widget();

        //this function is called each time the audio_widget is placed onto the playfield, so that
        //all childs are already laid out and realized
        virtual void post_creation_callback() {}

        //get frame for UI
        void get_underlaying_fixed_position(int& x, int& y);
        
        //css
        void set_css_style(std::string filename, std::string css_class);

        //set label
        void set_label(std::string label);

        //add port
        void add_port(port* p);
        std::vector<port*>* get_port_list();

    protected: 

        //pointer
        Glib::RefPtr<Gtk::GestureDrag> gesture_drag;

        Gtk::Label label;

        int ignore_mouse_drag;

        //child frame
        void mouse_grab_callback(int x, int y);
        void mouse_grab_update_callback(int offset_x, int offset_y);

        std::vector<port*> port_vector;

        //number of ports
        int n_out_ports = 0;
        int n_in_ports = 0;

        void on_port_realize(port* p);

        //position in the playfield
        int x_pos = 0;
        int y_pos = 0;

        //css provider
        Glib::RefPtr<Gtk::CssProvider> css_provider;
};