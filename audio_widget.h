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
#include "context.h"
#include "ui_control.h"

#include <iostream>
#include <fstream>
#include <string>

#include <gtk/gtk.h>

class audio_widget
{

    public:

        audio_widget(context* program_context, int x_pos = 300, int y_pos = 300);
        ~audio_widget();

        //this function is called when the audio_widget is created via the constructor
        virtual void on_creation_callback() = 0;

        //this function is called each time the audio_widget is placed onto the playfield, so that
        //all childs are already laid out and realized
        virtual void post_creation_callback() = 0;

        //this function is called for each sample calculated
        virtual void process() {}

        //this function is called independently for updating the ui in the ui thread
        virtual void process_ui() {}

        //UI PART
        void add_control(control_type type, std::string name, std::string parameter_name);
        void set_control_text(std::string name, std::string text);
        bool find_control_by_widget_pointer(GtkWidget* widget, ui_control& control);
        bool find_control_id(std::string name, int& id);
        void link_port_to_control(std::string linkname, std::string port, std::string control);
        void control_match_to_param(ui_control& ui);

        //PARAMETERS
        void add_parameter(std::string name, float starting_val);
        float get_parameter_value(std::string name);
        int find_parameter_id(std::string name);

        //settings
        //these go on the sidebar, not on the playfield ui, and there are more kinds!

        //get frame for UI
        void get_underlaying_fixed_position(int& x, int& y);
        
        //widget appearance
        void set_css_style(std::string filename, std::string css_class);
        void set_label(std::string label);
        void set_size_request(int w, int h);

        //add port
        void add_port(port* p);
        std::vector<port*>* get_port_list();
        bool find_port(std::string name, port ** p);
        bool find_port_id(std::string name, int& id);

        //set ready state
        void set_ready(bool ready);
        bool is_ready();

        GtkFixed* base_class;

    private: 

        struct serialize_container{
            char widget_id[48];
            char title[48];
            int x_pos;
            int y_pos;
            int n_params;
            int n_ports;
            int n_controls;
            int n_links;
        };

        struct serialize_port{
            int index;
            char name[48];
            port::port_type type;
        };

        struct parameter{
            std::string name;
            float value;
        };

        struct link{
            std::string name;
            int port_id;
            int control_id;
        };

        //audio_widget 
        std::string widget_id;

        //pointer
        GtkGestureDrag* gesture_drag;
        GtkLabel* label;

        int ignore_mouse_drag;

        //ready to process
        bool isReady;

        //child frame
        static void mouse_grab_callback(GtkGestureDrag* gdrag, double x, double y, audio_widget* widget);
        static void mouse_grab_update_callback(GtkGestureDrag* gdrag, double offset_x, double offset_y, audio_widget* widget);

        //events handling interface
        void _base_class_process();
        static void _base_class_post_creation_callback(GtkFixed* fxd, audio_widget* widget);
        static void _base_class_on_creation_callback(GtkFixed* fxd, audio_widget* widget);
        static void _base_class_button_control_callback(GtkButton* btn, audio_widget* widget);
        static void _base_class_scale_control_callback(GtkRange* scale, audio_widget* widget);

        std::vector<port*> port_vector;
        std::vector<ui_control> control_list;
        std::vector<parameter> parameter_list;
        std::vector<link> link_list;

        //number of ports
        int n_out_ports = 0;
        int n_in_ports = 0;

        void on_port_realize(port* p);

        //position in the playfield
        int x_pos = 0;
        int y_pos = 0;

        //css provider
        GtkCssProvider* css_provider;

        //context
        context* program_context;

        //file writing
        void serialize(std::ofstream& str);
};