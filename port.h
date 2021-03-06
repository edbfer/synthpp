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

#pragma once

#include <string>

#include <gtk/gtk.h>
#include <boost/circular_buffer.hpp>

class port
{
    public:

        enum port_type
        {
            INPUT,
            OUTPUT
        };

        port(std::string label = "noname", port_type p_type = port_type::OUTPUT);

        port_type get_direction();
        bool is_connected();
        void set_connected(bool connected);

        void set_position_inwidget(int x, int y);
        void get_position_inwidget(int& x, int& y);

        void set_position_indarea(int x, int y);
        void get_position_indarea(int& x, int& y);

        GtkAllocation get_allocation();
        std::string get_label();
        GtkLabel* get_base_class();

        void set_hovered(bool hover);
        void set_grabbed(bool grab);
        bool is_hovered();
        bool is_grabbed();

        float pop_sample();
        void push_sample(float sample);

    protected:

        GtkLabel* base_class;

        GtkGestureDrag* gesture_drag;

        static void mouse_grab_callback(GtkGestureDrag* gdrag, int x, int y, port* port);

        //position on the widget
        int x, y;
        port_type direction;
        bool ui_hovered;
        bool ui_grabbed;
        bool connected;

        //position on darea
        int darea_x, darea_y;

        //audio samples data
        //boost::circular_buffer<float> buffer;
        float sample;
};