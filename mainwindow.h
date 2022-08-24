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

#include <gtk/gtk.h>
#include <adwaita.h>

#include <vector>
#include <string>

#include "audio_widget.h"
#include "signal_path.h"
#include "audio_engine.h"
#include "port.h"
#include "midi_options_dialog.h"
#include "context.h"

#include "widget_manager.h"
#include "plugin_manager.h"
#include "settings_manager.h"

// Mainwindow class
//
class MainWindow
{
    public:

        //constructor
        MainWindow();
        ~MainWindow();

        //create layout
        static void create_layout(GtkApplication* app, MainWindow* window);

        //playfield functions
        void playfield_add_widget(audio_widget* awidget);
        void playfield_remove_widget(audio_widget* widget);
        void playfield_trigger_redraw();

        std::vector<audio_widget*>& get_audio_widget_list();
        std::vector<signal_path*>& get_signal_path_list();

        //function to write on the log window
        void log(std::string text);
    
    protected:

        GtkApplication* app;
        GtkWindow* main_window;
        //GtkPaned* main_grid;
        AdwFlap* main_grid;
        GtkSeparator* right_separator;
        AdwClamp* right_size_clamp;
        
        GtkScrolledWindow* playfield_scroll;
        GtkFixed* playfield;

        //add menu buttons
        GtkHeaderBar* header_bar;
        GtkButton* hbar_midi_options_button;
        GtkToggleButton* hbar_collapse_panel;

        //associated actions
        midi_options_dialog* midi_dialog = NULL;
        static void hbar_midi_options_button_clicked(GtkButton* btn, MainWindow* window);
        static void hbar_collapse_panel_button_toggled(GtkToggleButton* btn, MainWindow* window);

        GtkDrawingArea* playfield_aux_darea;
        bool darea_mouse_grabbed;
        double darea_start_mouse_x, darea_start_mouse_y;
        double darea_cur_mouse_x, darea_cur_mouse_y;

        int last_sidebar_position;
        GtkBox* right_panel;
        GtkLabel* log_label;
        GtkTextView* log_panel;

        //widget list
        GtkComboBoxText* widget_catalog;
        GtkComboBoxText* audio_device_catalog;
        static void audio_device_catalog_changed(GtkComboBox* cbx, MainWindow* window);

        GtkScrolledWindow* log_panel_scroll;

        //buttons
        GtkButton* test_button;

        //start audioengine button
        GtkButton* start_engine_button;
        GtkButton* stop_engine_button;
        //Engine buttons in the header bar
        GtkToggleButton* hbar_start_engine;
        static void hbar_start_engine_button_toggled(GtkToggleButton* btn, MainWindow* window);
        GtkButton* hbar_stop_engine;
        static void hbar_stop_engine_button_clicked(GtkButton* btn, MainWindow* window);

        //playfield list
        std::vector<audio_widget*> playfield_widget_list;

        static void test_button_clicked_callback(GtkButton* btn, MainWindow* window); 
        static void start_engine_button_clicked_callback(GtkButton* btn, MainWindow* window);   
        static void stop_engine_button_clicked_callback(GtkButton* btn, MainWindow* window);  
        static void playfield_aux_darea_draw(GtkDrawingArea *area, cairo_t *cr, int width, int height, MainWindow* window);

        static void scrolled_edge_reached(GtkScrolledWindow* self, GtkPositionType pos, MainWindow* window);
        static void mainwindow_show_callback(GtkWindow* w, MainWindow* window);
        static void gdk_surface_layout_callback(GdkSurface* sfc, int width, int height, MainWindow* window);

        bool do_redraw;
        static bool playfield_signal_redraw(MainWindow* window);
        
        static void playfield_aux_darea_gesture_click(GtkGestureClick* ges, int npresses, double x, double y, MainWindow* window);
        static void playfield_aux_darea_begin_grab(GtkGestureDrag* ges, double x, double y, MainWindow* window);
        static void playfield_aux_darea_update_grab(GtkGestureDrag* ges, double offset_x, double offset_y, MainWindow* window);
        static void playfield_aux_darea_end_grab(GtkGestureDrag* ges, double x, double y, MainWindow* window);
        static void playfield_aux_darea_motion(GtkEventControllerMotion* ecm, double x, double y, MainWindow* window);
        static bool playfield_aux_darea_key_down(GtkEventControllerKey* eck, guint keyval, guint keycode, GdkModifierType state, MainWindow* window);
        static void playfield_aux_darea_key_up(GtkEventControllerKey* eck, guint keyval, guint keycode, GdkModifierType state, MainWindow* window);

        //tentative path
        bool can_create_path;
        bool is_delete_key = false;
        //delete path function
        void remove_signal_path(signal_path* s_path);

        signal_path* candidate_path;
        port* starting_port;
        std::vector<signal_path*> signal_path_list;

        //cria a audioengine
        context* program_context;
        audio_engine* engine = nullptr;

        //create logger structure to pass
        settings_manager* smanager;
        widget_manager* wmanager;
        plugin_manager* pmanager;

        //widget manager
        void register_builtin_widgets();

        //actions
        void create_app_actions();
        GSimpleAction* app_add_widget_action;
        static void app_action_callback(GSimpleAction* act, GVariant* param, MainWindow* window);

        //popover for the drawing area
        GtkPopover* darea_popover;
        GtkScrolledWindow* darea_popover_sw;
        GtkListBox* darea_popover_lb;
        static void darea_popover_lv_fac_setup(GtkSignalListItemFactory* fac, GtkListItem* list_item, MainWindow* window);
        static void darea_popover_lv_fac_bind(GtkSignalListItemFactory* fac, GtkListItem* object, MainWindow* window);
        static void darea_popover_activated(GtkListBox* lb, GtkListBoxRow* row, MainWindow* window);
        void populate_darea_popover();
};
