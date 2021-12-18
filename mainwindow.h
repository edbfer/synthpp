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
#include "signal_path.h"
#include "port.h"

// Mainwindow class
//
class MainWindow : public Gtk::Window {

    public:

        //constructor
        MainWindow();

        //playfield functions
        void playfield_add_widget(audio_widget* awidget);
        void playfield_trigger_redraw();
    
    protected:
        Gtk::Grid main_grid;
        
        Gtk::ScrolledWindow playfield_scroll;
        Gtk::Fixed playfield;

        Gtk::DrawingArea playfield_aux_darea;
        bool darea_mouse_grabbed;
        double darea_start_mouse_x, darea_start_mouse_y;
        double darea_cur_mouse_x, darea_cur_mouse_y;

        Gtk::Grid right_panel;
        Gtk::Label log_label;
        Gtk::TextView log_panel;
        Gtk::ScrolledWindow log_panel_scroll;

        Gtk::Button test_button;

        //playfield list
        std::vector<audio_widget*> playfield_widget_list;

        void test_button_clicked_callback();    
        void playfield_aux_darea_draw(const Glib::RefPtr<Cairo::Context> cairo_context, int width, int height);

        void scrolled_edge_reached(Gtk::PositionType pos_type);
        void mainwindow_show_callback();
        void gdk_surface_layout_callback(int width, int height);

        bool do_redraw;
        bool playfield_signal_redraw();
        
        void playfield_aux_darea_begin_grab(double x, double y);
        void playfield_aux_darea_update_grab(double offset_x, double offset_y);
        void playfield_aux_darea_end_grab(double x, double y);
        void playfield_aux_darea_motion(double x, double y);

        //tentative path
        bool can_create_path;
        signal_path* candidate_path;
        port* starting_port;
        std::vector<signal_path*> signal_path_list;
};