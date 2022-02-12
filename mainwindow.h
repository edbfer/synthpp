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
#include <gtkmm/dropdown.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/headerbar.h>

#include <vector>
#include <string>

#include "audio_widget.h"
#include "signal_path.h"
#include "audio_engine.h"
#include "port.h"

// Mainwindow class
//
class MainWindow : public Gtk::Window 
{

    public:

        //constructor
        MainWindow();

        //playfield functions
        void playfield_add_widget(audio_widget* awidget);
        void playfield_remove_widget(audio_widget* widget);
        void playfield_trigger_redraw();

        std::vector<audio_widget*>& get_audio_widget_list();
        std::vector<signal_path*>& get_signal_path_list();
    
    protected:
        Gtk::Grid main_grid;
        
        Gtk::ScrolledWindow playfield_scroll;
        Gtk::Fixed playfield;

        //add menu buttons
        Gtk::HeaderBar header_bar;
        Gtk::Button hbar_midi_options_button;
        //associated actions
        void hbar_midi_options_button_clicked();

        Gtk::DrawingArea playfield_aux_darea;
        bool darea_mouse_grabbed;
        double darea_start_mouse_x, darea_start_mouse_y;
        double darea_cur_mouse_x, darea_cur_mouse_y;

        Gtk::Grid right_panel;
        Gtk::Label log_label;
        Gtk::TextView log_panel;

        //widget list
        Gtk::ComboBoxText widget_catalog;
        Gtk::ComboBoxText audio_device_catalog;
        void audio_device_catalog_changed();
        
        //function to write on the log window
        void log(std::string text);

        Gtk::ScrolledWindow log_panel_scroll;

        //buttons
        Gtk::Button test_button;

        //start audioengine button
        Gtk::Button start_engine_button;
        Gtk::Button stop_engine_button;

        //playfield list
        std::vector<audio_widget*> playfield_widget_list;

        void test_button_clicked_callback(); 
        void start_engine_button_clicked_callback();   
        void stop_engine_button_clicked_callback();  
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
        bool playfield_aux_darea_key_down(guint keyval, guint keycode, Gdk::ModifierType state);
        void playfield_aux_darea_key_up(guint keyval, guint keycode, Gdk::ModifierType state);

        //tentative path
        bool can_create_path;
        bool is_delete_key = false;
        //delete path function
        void remove_signal_path(signal_path* s_path);

        signal_path* candidate_path;
        port* starting_port;
        std::vector<signal_path*> signal_path_list;

        //cria a audioengine
        audio_engine* engine = nullptr;

        //create logger structure to pass
        context program_context;
};
