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

#include <gtkmm/window.h>
#include <gtkmm/grid.h>
#include <gtkmm/fixed.h>
#include <gtkmm/paned.h>
#include <gtkmm/label.h>
#include <gtkmm/textview.h>
#include <gtkmm/frame.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/button.h>
#include <gtkmm/drawingarea.h>

#include <vector>

#include "audio_widget.h"

// Mainwindow class
//
class MainWindow : public Gtk::Window {

    public:

        //constructor
        MainWindow();

        //playfield functions
        void playfield_add_widget(audio_widget& awidget);

    protected:
        Gtk::Grid main_grid;
        
        Gtk::ScrolledWindow playfield_scroll;
        Gtk::Fixed playfield;
        Gtk::DrawingArea playfield_aux_darea;

        Gtk::Grid right_panel;
        Gtk::Label log_label;
        Gtk::TextView log_panel;
        Gtk::ScrolledWindow log_panel_scroll;

        Gtk::Button test_button;

        //playfield list
        std::vector<audio_widget*> playfield_widget_list;

        void test_button_clicked_callback();
        void playfield_aux_darea_draw(const Glib::RefPtr<Cairo::Context> cairo_context, int width, int height);

        void scrolled_edge_reached();
};