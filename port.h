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

#include <gtkmm/label.h>
#include <gtkmm/gesturedrag.h>

class port : public Gtk::Label
{
    public:

        enum port_type
        {
            INPUT,
            OUTPUT
        };

        port(std::string label = "noname", port_type p_type = port_type::OUTPUT);

        port_type get_direction();
        void set_position_inwidget(int x, int y);
        void get_position_inwidget(int& x, int& y);
        void set_hovered(bool hover);
        void set_grabbed(bool grab);
        bool is_hovered();
        bool is_grabbed();

    protected:

        Glib::RefPtr<Gtk::GestureDrag> gesture_drag;

        void mouse_grab_callback(int x, int y);

        //position on the widget
        int x, y;
        port_type direction;
        bool ui_hovered;
        bool ui_grabbed;
};