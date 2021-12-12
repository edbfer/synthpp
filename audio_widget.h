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

#include <iostream>

#include <gtkmm/frame.h>
#include <gtkmm/fixed.h>
#include <gtkmm/gesturedrag.h>

class audio_widget : public Gtk::Frame
{

    public:

        audio_widget();
        ~audio_widget();

        //get frame for UI
        Glib::RefPtr<Gtk::Frame> get_underlaying_frame();

    protected: 

        //pointer
        Glib::RefPtr<Gtk::GestureDrag> gesture_drag;

        //child frame
        void mouse_grab_callback(int x, int y);
        void mouse_grab_update_callback(int offset_x, int offset_y);

};