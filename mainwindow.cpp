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

#include "mainwindow.h"
#include "debug_widget.h"
#include "counter_widget.h"
#include "probe_widget.h"
#include "port.h"
#include "delay_widget.h"
#include "feedback_delay_widget.h"
#include "gain_widget.h"

//dialogs
#include "midi_options_dialog.h"
#include "settings_manager.h"
#include "widget_manager.h"
#include "plugin_manager.h"

#include <dlfcn.h>
#include <pthread.h>
#include <algorithm>
#include <tuple>
#include <cstring>

#include <gdk/gdkcairocontext.h>
#include <gdk/gdkcairo.h>
//#include <cairo/gtcairomm.h>
#include <gdk/gdksurface.h>
#include <glib.h>


MainWindow::MainWindow()
{

}

MainWindow::~MainWindow()
{
   gtk_window_destroy(main_window);
}

void MainWindow::create_layout(GtkApplication* app, MainWindow* window){

    //create window
    window->app = app;
    window->main_window = (GtkWindow*) gtk_application_window_new(app);
    gtk_window_set_title(window->main_window, "synthpp");

    //do the remainder of the init
    window->smanager = new settings_manager();
    window->smanager->load_settings();

    window->wmanager = new widget_manager();
    window->register_builtin_widgets();

    window->pmanager = new plugin_manager(window->smanager, window->wmanager);
    window->pmanager->load_plugins();
    window->create_app_actions();

    //initialize vars
    window->can_create_path = false;
    window->is_delete_key = false;
    window->darea_mouse_grabbed = false;

    //actual ui
    int height = 768, width = 1024;
    gtk_window_set_default_size(window->main_window, width, height);

    //do the header bar
    window->header_bar = (GtkHeaderBar*) gtk_header_bar_new();
    gtk_window_set_titlebar(window->main_window, GTK_WIDGET(window->header_bar));

    //midi_options
    window->hbar_midi_options_button = (GtkButton*) gtk_button_new();
    gtk_button_set_label(window->hbar_midi_options_button, "MIDI Options");
    gtk_widget_set_hexpand(GTK_WIDGET(window->hbar_midi_options_button), true);
    g_signal_connect(window->hbar_midi_options_button, "clicked", G_CALLBACK(hbar_midi_options_button_clicked), window);
    gtk_header_bar_pack_end(window->header_bar, GTK_WIDGET(window->hbar_midi_options_button));

    //collapse button
    window->hbar_collapse_panel = (GtkToggleButton*) gtk_toggle_button_new();
    gtk_button_set_label(GTK_BUTTON(window->hbar_collapse_panel), "Collapse");
    gtk_widget_set_hexpand(GTK_WIDGET(window->hbar_collapse_panel), true);
    gtk_widget_set_vexpand(GTK_WIDGET(window->hbar_collapse_panel), true);
    gtk_button_set_icon_name(GTK_BUTTON(window->hbar_collapse_panel), "sidebar-collapse");
    gtk_toggle_button_set_active(window->hbar_collapse_panel, true);
    g_signal_connect(window->hbar_collapse_panel, "toggled", G_CALLBACK(hbar_collapse_panel_button_toggled), window);
    gtk_header_bar_pack_start(window->header_bar, GTK_WIDGET(window->hbar_collapse_panel));

    //play hbar
    window->hbar_start_engine = (GtkToggleButton*) gtk_toggle_button_new();
    gtk_button_set_label(GTK_BUTTON(window->hbar_start_engine), "Start");
    gtk_widget_set_hexpand(GTK_WIDGET(window->hbar_start_engine), true);
    gtk_widget_set_vexpand(GTK_WIDGET(window->hbar_start_engine), true);
    gtk_button_set_icon_name(GTK_BUTTON(window->hbar_start_engine), "stock_media-play");
    g_signal_connect(window->hbar_start_engine, "toggled", G_CALLBACK(hbar_start_engine_button_toggled), window);
    gtk_header_bar_pack_start(window->header_bar, GTK_WIDGET(window->hbar_start_engine));

    //stop hbar
    window->hbar_stop_engine = (GtkButton*) gtk_button_new();
    gtk_button_set_label(window->hbar_stop_engine, "Stop");
    gtk_widget_set_hexpand(GTK_WIDGET(window->hbar_stop_engine), true);
    gtk_widget_set_vexpand(GTK_WIDGET(window->hbar_stop_engine), true);
    gtk_button_set_icon_name(window->hbar_stop_engine, "stock_media-stop");
    g_signal_connect(window->hbar_stop_engine, "clicked", G_CALLBACK(stop_engine_button_clicked_callback), window);
    gtk_header_bar_pack_start(window->header_bar, GTK_WIDGET(window->hbar_stop_engine));

    //create the grid widget
    window->main_grid = reinterpret_cast<AdwOverlaySplitView*>(adw_overlay_split_view_new());
    adw_overlay_split_view_set_collapsed(window->main_grid, true);
    //gtk_orientable_set_orientation(GTK_ORIENTABLE(window->main_grid), GTK_ORIENTATION_HORIZONTAL);

    gtk_window_set_child(window->main_window, GTK_WIDGET(window->main_grid));
    //gtk_widget_set_halign(GTK_WIDGET(window->main_grid), GTK_ALIGN_FILL);
    gtk_widget_set_valign(GTK_WIDGET(window->main_grid), GTK_ALIGN_FILL);
    //gtk_widget_set_hexpand(GTK_WIDGET(window->main_grid), true);
    gtk_widget_set_vexpand(GTK_WIDGET(window->main_grid), true);

    window->right_separator = (GtkSeparator*) gtk_separator_new(GTK_ORIENTATION_VERTICAL);

    //right panel
    //stack
    window->right_stack = (GtkStack*) gtk_stack_new();
    //log panel as stack page
    window->right_panel = (GtkBox*) gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_size_request(GTK_WIDGET(window->right_panel), 350, -1);
    gtk_stack_add_named(window->right_stack, GTK_WIDGET(window->right_panel), "logpanel");

    //right properties 
    window->right_preferences = (AdwPreferencesPage*) adw_preferences_page_new();
    gtk_stack_add_named(window->right_stack, GTK_WIDGET(window->right_preferences), "wprefs");
    //set log as def
    gtk_stack_set_visible_child_name(window->right_stack, "logpanel");

    window->right_size_clamp = (AdwClamp*) adw_clamp_new();
    gtk_orientable_set_orientation(GTK_ORIENTABLE(window->right_size_clamp), GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_size_request(GTK_WIDGET(window->right_size_clamp), 100, -1);
    adw_clamp_set_maximum_size(window->right_size_clamp, 100);

    adw_overlay_split_view_set_sidebar(window->main_grid, GTK_WIDGET(window->right_stack));

    //gtk_paned_set_start_child(window->main_grid, GTK_WIDGET(window->right_panel));
    window->last_sidebar_position = (1./4.) * width;
    //gtk_paned_set_position(window->main_grid, window->last_sidebar_position);
    adw_overlay_split_view_set_sidebar_position(window->main_grid, GTK_PACK_START);

    gtk_widget_set_margin_end(GTK_WIDGET(window->right_panel), 10);
    gtk_widget_set_margin_top(GTK_WIDGET(window->right_panel), 10);
    gtk_widget_set_margin_start(GTK_WIDGET(window->right_panel), 10);
    gtk_widget_set_margin_bottom(GTK_WIDGET(window->right_panel), 10);

    //right panel/log label
    window->log_label = (GtkLabel*) gtk_label_new("");
    gtk_box_prepend(window->right_panel, GTK_WIDGET(window->log_label));
    gtk_label_set_text(window->log_label, "Log window");
    gtk_widget_set_margin_start(GTK_WIDGET(window->log_label), 5);
    gtk_widget_set_margin_end(GTK_WIDGET(window->log_label), 5);
    gtk_widget_set_can_focus(GTK_WIDGET(window->log_label), false);
    gtk_label_set_justify(window->log_label, GTK_JUSTIFY_CENTER);

    //log view window
    window->log_panel_scroll = (GtkScrolledWindow*) gtk_scrolled_window_new();
    gtk_box_append(window->right_panel, GTK_WIDGET(window->log_panel_scroll));
    gtk_widget_set_vexpand(GTK_WIDGET(window->right_panel), true);
    window->log_panel = (GtkTextView*) gtk_text_view_new();
    gtk_scrolled_window_set_child(window->log_panel_scroll, GTK_WIDGET(window->log_panel));
    gtk_widget_set_vexpand(GTK_WIDGET(window->log_panel), true);
    gtk_text_view_set_wrap_mode(window->log_panel, GTK_WRAP_CHAR);
    gtk_widget_set_can_focus(GTK_WIDGET(window->log_panel), false);

    //audio device catalog
    window->audio_device_catalog = (GtkComboBoxText*) gtk_combo_box_text_new();
    gtk_widget_set_margin_start(GTK_WIDGET(window->audio_device_catalog), 5);
    gtk_widget_set_margin_end(GTK_WIDGET(window->audio_device_catalog), 5);
    g_signal_connect(window->audio_device_catalog, "changed", G_CALLBACK(audio_device_catalog_changed), window);
    gtk_box_append(window->right_panel, GTK_WIDGET(window->audio_device_catalog));

    //start engine
    window->start_engine_button = (GtkButton*) gtk_button_new();
    gtk_button_set_label(window->start_engine_button, "Start Audio Engine");
    gtk_widget_set_margin_start(GTK_WIDGET(window->start_engine_button), 5);
    gtk_widget_set_margin_end(GTK_WIDGET(window->start_engine_button), 5);
    g_signal_connect(window->start_engine_button, "clicked", G_CALLBACK(start_engine_button_clicked_callback), window);

    //stop engine
    window->stop_engine_button = (GtkButton*) gtk_button_new();
    gtk_button_set_label(window->stop_engine_button, "Stop Audio Engine");
    gtk_widget_set_margin_start(GTK_WIDGET(window->stop_engine_button), 5);
    gtk_widget_set_margin_end(GTK_WIDGET(window->stop_engine_button), 5);
    g_signal_connect(window->stop_engine_button, "clicked", G_CALLBACK(stop_engine_button_clicked_callback), window);

    gtk_box_append(window->right_panel, GTK_WIDGET(window->start_engine_button));
    gtk_box_append(window->right_panel, GTK_WIDGET(window->stop_engine_button));

    //place the fixed
    window->playfield = (GtkFixed*) gtk_fixed_new();
    gtk_widget_set_hexpand(GTK_WIDGET(window->playfield), true);
    gtk_widget_set_vexpand(GTK_WIDGET(window->playfield), true);
    gtk_widget_set_halign(GTK_WIDGET(window->playfield), GTK_ALIGN_FILL);
    gtk_widget_set_valign(GTK_WIDGET(window->playfield), GTK_ALIGN_FILL);
    gtk_widget_set_margin_start(GTK_WIDGET(window->playfield), 0);
    gtk_widget_set_margin_top(GTK_WIDGET(window->playfield), 0);
    gtk_widget_set_margin_end(GTK_WIDGET(window->playfield), 0);
    gtk_widget_set_margin_bottom(GTK_WIDGET(window->playfield), 0);

    //playfield scroll
    window->playfield_scroll = (GtkScrolledWindow*) gtk_scrolled_window_new();
    gtk_widget_set_hexpand(GTK_WIDGET(window->playfield_scroll), true);
    gtk_widget_set_vexpand(GTK_WIDGET(window->playfield_scroll), true);
    gtk_scrolled_window_set_child(window->playfield_scroll, GTK_WIDGET(window->playfield));
    gtk_widget_set_halign(GTK_WIDGET(window->playfield_scroll), GTK_ALIGN_FILL);
    gtk_widget_set_valign(GTK_WIDGET(window->playfield_scroll), GTK_ALIGN_FILL);
    g_signal_connect(window->playfield_scroll, "edge-reached", G_CALLBACK(scrolled_edge_reached), window);
    adw_overlay_split_view_set_content(window->main_grid, GTK_WIDGET(window->playfield_scroll));
    //gtk_paned_set_end_child(window->main_grid, GTK_WIDGET(window->playfield_scroll));

    //drawing area
    window->playfield_aux_darea = (GtkDrawingArea*) gtk_drawing_area_new();
    gtk_drawing_area_set_draw_func(window->playfield_aux_darea, GtkDrawingAreaDrawFunc(playfield_aux_darea_draw), window, NULL);
    gtk_widget_set_size_request(GTK_WIDGET(window->playfield_aux_darea), width, height);
    //get size of fixed
    gtk_fixed_put(window->playfield, GTK_WIDGET(window->playfield_aux_darea), 0, 0);

    //darea popover
    window->darea_popover = (GtkPopover*) gtk_popover_new();
    gtk_widget_set_hexpand(GTK_WIDGET(window->darea_popover), true);
    gtk_widget_set_vexpand(GTK_WIDGET(window->darea_popover), true);
    gtk_widget_set_halign(GTK_WIDGET(window->darea_popover), GTK_ALIGN_FILL);
    gtk_widget_set_valign(GTK_WIDGET(window->darea_popover), GTK_ALIGN_FILL);
    /*window->darea_popover_sw = (GtkScrolledWindow*) gtk_scrolled_window_new();
    gtk_widget_set_halign(GTK_WIDGET(window->darea_popover_sw), GTK_ALIGN_FILL);
    gtk_widget_set_valign(GTK_WIDGET(window->darea_popover_sw), GTK_ALIGN_FILL);
    gtk_widget_set_hexpand(GTK_WIDGET(window->darea_popover_sw), true);
    gtk_popover_set_child(window->darea_popover, GTK_WIDGET(window->darea_popover_sw));*/

    window->darea_popover_lb = (GtkListBox*) gtk_list_box_new();
    gtk_list_box_set_selection_mode(window->darea_popover_lb, GTK_SELECTION_NONE);
    gtk_widget_set_halign(GTK_WIDGET(window->darea_popover_lb), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(window->darea_popover_lb), GTK_ALIGN_FILL);
    gtk_widget_set_hexpand(GTK_WIDGET(window->darea_popover_lb), true);
    gtk_widget_set_vexpand(GTK_WIDGET(window->darea_popover_lb), true);
    /*gtk_scrolled_window_set_child(window->darea_popover_sw, GTK_WIDGET(window->darea_popover_lb));*/
    g_signal_connect(window->darea_popover_lb, "row-activated", G_CALLBACK(darea_popover_activated), window);
    gtk_popover_set_child(window->darea_popover, GTK_WIDGET(window->darea_popover_lb));

    //list store for the popover
    window->populate_darea_popover();

    gtk_popover_set_has_arrow(GTK_POPOVER(window->darea_popover), false);
    gtk_fixed_put(window->playfield, GTK_WIDGET(window->darea_popover), 200, 200);

    //show eevent
    g_signal_connect(window->main_window, "show", G_CALLBACK(mainwindow_show_callback), window);
    //trigger redraws
    g_timeout_add(17, GSourceFunc(playfield_signal_redraw), window);

    //attach drawing area mouse controller
    GtkGestureClick* darea_gesture_click = (GtkGestureClick*) gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(darea_gesture_click), 0);
    gtk_widget_add_controller(GTK_WIDGET(window->playfield_aux_darea),
    GTK_EVENT_CONTROLLER(darea_gesture_click));

    GtkGestureDrag* darea_gesture_drag = (GtkGestureDrag*) gtk_gesture_drag_new();
    gtk_widget_add_controller(GTK_WIDGET(window->playfield_aux_darea), GTK_EVENT_CONTROLLER(darea_gesture_drag));

    GtkEventControllerMotion* darea_mouse_motion = (GtkEventControllerMotion*) gtk_event_controller_motion_new();
    gtk_widget_add_controller(GTK_WIDGET(window->playfield_aux_darea), GTK_EVENT_CONTROLLER(darea_mouse_motion));

    g_signal_connect(darea_gesture_click, "pressed", G_CALLBACK(playfield_aux_darea_gesture_click), window);
    g_signal_connect(darea_mouse_motion, "motion", G_CALLBACK(playfield_aux_darea_motion), window);
    g_signal_connect(darea_gesture_drag, "drag-begin", G_CALLBACK(playfield_aux_darea_begin_grab), window);
    g_signal_connect(darea_gesture_drag, "drag-update", G_CALLBACK(playfield_aux_darea_update_grab), window);
    g_signal_connect(darea_gesture_drag, "drag-end", G_CALLBACK(playfield_aux_darea_end_grab), window);

    //grab keyboard events
    GtkEventControllerKey* darea_key_actions = (GtkEventControllerKey*) gtk_event_controller_key_new();
    gtk_widget_add_controller(GTK_WIDGET(window->main_window), GTK_EVENT_CONTROLLER(darea_key_actions));

    g_signal_connect(darea_key_actions, "key-pressed", G_CALLBACK(playfield_aux_darea_key_down), window);
    g_signal_connect(darea_key_actions, "key-released", G_CALLBACK(playfield_aux_darea_key_up), window);

    //memset the program_context, for some reason is being stupid
    //std::memset(&window->program_context, 0, sizeof(program_context));
    /*window->program_context.log = sigc::mem_fun(*window, &MainWindow::log);
    window->program_context.put_widget = sigc::mem_fun(*window, &MainWindow::playfield_add_widget);
    window->program_context.remove_widget = sigc::mem_fun(*window, &MainWindow::playfield_remove_widget);*/

    //present the window
    gtk_window_present(window->main_window);
}

void MainWindow::playfield_add_widget(audio_widget* awidget)
{
    playfield_widget_list.push_back(awidget);

    gtk_fixed_put(playfield, GTK_WIDGET(awidget->base_class), 300, 300);

    gtk_widget_show(GTK_WIDGET(awidget->base_class));

    gtk_widget_queue_draw(GTK_WIDGET(playfield_aux_darea));
}

void MainWindow::playfield_remove_widget(audio_widget* awidget)
{
    playfield_widget_list.erase(std::remove(playfield_widget_list.begin(), playfield_widget_list.end(), awidget), playfield_widget_list.end());

    gtk_fixed_remove(playfield, GTK_WIDGET(awidget->base_class));
}

void MainWindow::register_builtin_widgets()
{
    wmanager->register_widget("synthpp.builtin.debug", "Debug widget", "Just a simple debug widget", debug_widget::create_instance);
    wmanager->register_widget("synthpp.builtin.delay", "Simple delay", "Simple delay", delay_widget::create_instance);
    wmanager->register_widget("synthpp.builtin.fdelay", "Feedback delay", "Delay with feedback and dry/wet selectors", feedback_delay_widget::create_instance);
    wmanager->register_widget("synthpp.builtin.gain", "Gain boost", "Adjusts signal loudness", gain_widget::create_instance);
    wmanager->register_widget("synthpp.builtin.c4", "Sequencer: 4 bits", "Sequencer with 4 bits of output. Adjustable rate", counter_widget::create_instance_4);
    wmanager->register_widget("synthpp.builtin.c8", "Sequencer: 8 bits", "Sequencer with 8 bits of output. Adjustable rate", counter_widget::create_instance_8);
    wmanager->register_widget("synthpp.builtin.c16", "Sequencer: 16 bits", "Sequencer with 16 bits of output. Adjustable rate", counter_widget::create_instance_16);
    wmanager->register_widget("synthpp.builtin.probe", "Probe", "Shows the value of the connected path", probe_widget::create_instance);
}

void MainWindow::test_button_clicked_callback(GtkButton* btn, MainWindow* window)
{
    audio_widget* to_add;

    std::string selection = gtk_combo_box_get_active_id(GTK_COMBO_BOX(window->widget_catalog));

    to_add = window->wmanager->create_widget(selection);
    window->playfield_add_widget(to_add);
    
    //redraw
    gtk_widget_queue_draw(GTK_WIDGET(window->playfield_aux_darea));
}

void MainWindow::playfield_aux_darea_draw(GtkDrawingArea *area, cairo_t *cr, int width, int height, MainWindow* window)
{
    int peg_radius = 6;
    int bias = 2;

    cairo_save(cr);

    cairo_set_source_rgb(cr, 0.25/4., 0.125/4., 0.0);
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_fill_preserve(cr);

    cairo_restore(cr);
    cairo_new_path(cr);

    for (audio_widget* aw : window->playfield_widget_list)
    {
        std::vector<port*> p_list = *(aw->get_port_list());
        for(port* p : p_list)
        {
            int p_x, p_y;
            p->get_position_indarea(p_x, p_y);

            int x = p_x + ((p->get_direction() == port::port_type::OUTPUT) ? peg_radius : -peg_radius);

            int y = p_y + peg_radius + bias;

            if(p->is_hovered())
            {
                cairo_set_source_rgb(cr, 1.0, 0.5, 0.0);
            }
            else if(p->is_grabbed())
            {
                cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
            }
            else
            {
                if(p->get_direction() == port::port_type::OUTPUT)
                    cairo_set_source_rgb(cr, 0.0, 1.0, 0.0);
                else if(p->get_direction() == port::port_type::INPUT)
                    cairo_set_source_rgb(cr, 0.0, 1.0, 0.0);
            }
            cairo_arc(cr, x, y, peg_radius, 0, 2.*M_PI);
            cairo_fill(cr);

            if(p->is_grabbed())
            {
                cairo_move_to(cr, x, y);
                cairo_line_to(cr, window->darea_cur_mouse_x, window->darea_cur_mouse_y);
                cairo_set_line_width(cr, 4);

                //do we have a path?
                if(window->can_create_path)
                {
                    //add tooltip

                    int mid_x = (x + window->darea_cur_mouse_x)/2;
                    int mid_y = (y + window->darea_cur_mouse_y)/2;
                    cairo_move_to(cr, mid_x, mid_y);
                    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
                    cairo_show_text(cr, ("Create path: " + window->candidate_path->get_path_name()).c_str());

                    cairo_set_source_rgb(cr, 0.0, 0.0, 1.0);
                }
                cairo_stroke(cr);
            }
        }
    }

    //now draw the paths
    for(signal_path* s_path : window->signal_path_list)
    {
        int source_x, source_y;
        int dest_x, dest_y;

        s_path->get_source_port()->get_position_indarea(source_x, source_y);
        s_path->get_destination_port()->get_position_indarea(dest_x, dest_y);

        source_x += peg_radius; //input
        dest_x -= peg_radius; //output

        source_y += peg_radius + bias;
        dest_y += peg_radius + bias;

        //label
        int mid_x = (source_x + dest_x)/2;
        int mid_y = (source_y + dest_y)/2;
        cairo_move_to(cr, mid_x, mid_y);

        //is the path selected?
        if(s_path->is_ui_selected())
        {
            cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
            cairo_show_text(cr, ("delete path_" + s_path->get_path_name()).c_str());
        }
        else
        {
            cairo_set_source_rgb(cr, 0.0, 1.0, 0.0);
            cairo_show_text(cr, ("path_" + s_path->get_path_name()).c_str());
        }

        cairo_move_to(cr, source_x, source_y);
        cairo_line_to(cr, dest_x, dest_y);      
        cairo_set_line_width(cr, 4);
        cairo_stroke(cr);  
    }
}

void MainWindow::gdk_surface_layout_callback(GdkSurface* sfc, int width, int height, MainWindow* window)
{
    //std::cout << "New size: " << width << ", " << height << std::endl;
    //increase the darea size, if needed
    GtkAllocation alloc;
    gtk_widget_get_allocation(GTK_WIDGET(window->playfield_aux_darea), &alloc);
    int cur_width = alloc.width;//playfield_aux_darea.get_allocation().get_width();
    int cur_height = alloc.height; //.get_allocation().get_height();

    int new_width = std::max(cur_width, width);
    int new_height = std::max(cur_height, height);

    gtk_widget_set_size_request(GTK_WIDGET(window->playfield_aux_darea), new_width, new_height);
}

void MainWindow::mainwindow_show_callback(GtkWindow* wnd, MainWindow* window)
{
    window->program_context = new context();
    window->program_context->set_base_class(window);

    //attach to window size change event
    auto gdk_surface = gtk_native_get_surface(GTK_NATIVE(wnd));

    g_signal_connect(gdk_surface, "layout", G_CALLBACK(gdk_surface_layout_callback), window);

    window->engine = new audio_engine(window->program_context, &(window->playfield_widget_list), &(window->signal_path_list));

    //create dialogs
    window->midi_dialog = new midi_options_dialog(window->main_window, window->engine);

    //clip text
    GtkAllocation alloc;
    gtk_widget_get_allocation(GTK_WIDGET(window->audio_device_catalog), &alloc);
    int width = alloc.width;

    GtkCellRendererText* cell_renderer = GTK_CELL_RENDERER_TEXT(g_list_first(gtk_cell_layout_get_cells(GTK_CELL_LAYOUT(window->audio_device_catalog)))->data);

    GValue w_chars = G_VALUE_INIT;
    g_value_init(&w_chars, G_TYPE_INT); g_value_set_int(&w_chars, 1);
    GValue em = G_VALUE_INIT; 
    g_value_init(&em, G_TYPE_INT); g_value_set_int(&em, PANGO_ELLIPSIZE_END);

    g_object_set_property(G_OBJECT(cell_renderer), "width-chars", &w_chars);
    g_object_set_property(G_OBJECT(cell_renderer), "ellipsize", &em);
    gtk_cell_renderer_set_fixed_size(GTK_CELL_RENDERER(cell_renderer), width, -1);

    //populate device list
    int position = 0;
    for(std::pair info_pair : *(window->engine->get_device_vector()))
    {
        gtk_combo_box_text_insert(window->audio_device_catalog, position++, std::to_string(position).c_str(), (std::string(info_pair.second->name) + " (in: " + std::to_string(info_pair.second->maxInputChannels) + ", out: " + std::to_string(info_pair.second->maxOutputChannels) + ")").c_str());
    }
}

void MainWindow::scrolled_edge_reached(GtkScrolledWindow* self, GtkPositionType pos, MainWindow* window)
{
    GtkAllocation darea_alloc;
    gtk_widget_get_allocation(GTK_WIDGET(window->playfield_aux_darea), &darea_alloc);
    int cur_width = darea_alloc.width;
    int cur_height = darea_alloc.height;

    GtkAllocation playfield_alloc;
    gtk_widget_get_allocation(GTK_WIDGET(window->playfield), &playfield_alloc);
    int playfield_width = playfield_alloc.width;
    int playfield_height = playfield_alloc.height;

    int new_w = std::max(cur_width, playfield_width);
    int new_h = std::max(cur_height, playfield_height);

    gtk_widget_set_size_request(GTK_WIDGET(window->playfield_aux_darea), new_w, new_h);
}

void MainWindow::playfield_trigger_redraw()
{
    do_redraw = true;
    //return true;
}

bool MainWindow::playfield_signal_redraw(MainWindow* window)
{
    gtk_widget_queue_draw(GTK_WIDGET(window->playfield_aux_darea));

    //also, ask if widgets want to redraw
    for(audio_widget* aw : window->playfield_widget_list)
    {
        aw->process_ui();
    }

    return true;
}

void MainWindow::playfield_aux_darea_begin_grab(GtkGestureDrag* ges, double x, double y, MainWindow* window)
{
    //are we deleting?
    if(window->is_delete_key)
    {   
        //check if any paths need deleting
        for(signal_path* path : window->signal_path_list)
        {
            if(path->is_ui_selected())
            {
                window->remove_signal_path(path);
                delete path;
            }
            else
                path->set_ui_selected(false);
        }
    }
    else
    {
        //look if one of the pegs is near, if it is then mark it
        if(window->darea_mouse_grabbed == false)
        {
            //check if we are not grabbed already
            for (audio_widget* aw : window->playfield_widget_list)
            {
                int peg_radius = 6;
                int bias = 2;

                std::vector<port*> p_list = *(aw->get_port_list());
                for(port* p : p_list)
                {
                    //por def, we set_hovered(false)
                    p->set_grabbed(false);

                    int p_x, p_y;
                    p->get_position_indarea(p_x, p_y);

                    int peg_x = p_x + ((p->get_direction() == port::port_type::OUTPUT) ? peg_radius : -peg_radius);
                    int peg_y = p_y + peg_radius + bias;

                    double distance = hypot(peg_x - x, peg_y - y);
                    if (distance < peg_radius)
                    {
                        //we are in, mark as occupied
                        p->set_grabbed(true);
                        window->starting_port = p;
                        window->darea_mouse_grabbed = true;
                        window->darea_start_mouse_x = window->darea_cur_mouse_x = x;
                        window->darea_start_mouse_y = window->darea_cur_mouse_y = y;

                        //create a new path
                        window->can_create_path = false;
                        window->candidate_path = new signal_path;
                    }
                }
            }
        }
    }
}

void MainWindow::playfield_aux_darea_update_grab(GtkGestureDrag* ges, double offset_x, double offset_y, MainWindow* window)
{
    window->darea_cur_mouse_x = window->darea_start_mouse_x + offset_x;
    window->darea_cur_mouse_y = window->darea_start_mouse_y + offset_y;

    //clear the general connection creation state
    window->can_create_path = false;

    //are we near a connection?
    for(audio_widget *aw : window->playfield_widget_list)
    {
        int peg_radius = 6;
        int bias = 2;

        for(port *p : *(aw->get_port_list()))
        {
            int p_x, p_y;
            p->get_position_indarea(p_x, p_y);

            int peg_x = p_x + ((p->get_direction() == port::port_type::OUTPUT) ? peg_radius : -peg_radius);
            int peg_y = p_y + peg_radius + bias;

            double distance = hypot(peg_x - window->darea_cur_mouse_x, peg_y - window->darea_cur_mouse_y);
            
            if(distance < peg_radius)
            {
                //we are near another peg
                //can we attach?
                signal_path* tentative = window->candidate_path;

                if(window->starting_port->get_direction() == port::port_type::INPUT && p->get_direction() == port::port_type::OUTPUT)
                {
                    tentative->set_source_port(p);
                    tentative->set_destination_port(window->starting_port);
                    window->can_create_path = true;
                }
                else if(window->starting_port->get_direction() == port::port_type::OUTPUT && p->get_direction() == port::port_type::INPUT)
                {
                    tentative->set_source_port(window->starting_port);
                    tentative->set_destination_port(p);
                    window->can_create_path = true;
                }
            }
        }
    }

    //playfield_aux_darea.queue_draw();
}

void MainWindow::playfield_aux_darea_end_grab(GtkGestureDrag* ges, double x, double y, MainWindow* window)
{
    if(window->darea_mouse_grabbed)
    {
        for(audio_widget* aw : window->playfield_widget_list)
        {
            std::vector<port*> p_list = *(aw->get_port_list());
            for(port* p : p_list)
            {
                p->set_grabbed(false);
            }
        }

        window->darea_mouse_grabbed = false;
        //darea_mouse_x = 0;
        //darea_mouse_y = 0;

        //can we create path?
        if(window->can_create_path)
            window->signal_path_list.push_back(window->candidate_path);
        else
            delete window->candidate_path;

        //playfield_aux_darea.queue_draw();
    }
}

void MainWindow::playfield_aux_darea_motion(GtkEventControllerMotion* ecm, double x, double y, MainWindow* window)
{
    //are we on the delete mode?
    if(window->is_delete_key)
    {
        for(signal_path* path : window->signal_path_list)
        {
            path->set_ui_selected(false);

            //is the path being hovered?
            if(path->bounds_check(x, y, 15.0f))
                path->set_ui_selected(true);
        }
    }
    else
    {
        //look if one of the pegs is near, if it is then mark it
        for (audio_widget* aw : window->playfield_widget_list)
        {
            int peg_radius = 6;
            int bias = 2;

            std::vector<port*> p_list = *(aw->get_port_list());
            for(port* p : p_list)
            {
                //por def, we set_hovered(false)
                p->set_hovered(false);

                int p_x, p_y;
                p->get_position_indarea(p_x, p_y);

                int peg_x = p_x + ((p->get_direction() == port::port_type::OUTPUT) ? peg_radius : -peg_radius);
                int peg_y = p_y + peg_radius + bias;

                double distance = hypot(peg_x - x, peg_y - y);
                if (distance < peg_radius)
                {
                    //we are in, marca como occupied
                    p->set_hovered(true);

                    //playfield_aux_darea.queue_draw();
                }
            }
        }
    }
}

void MainWindow::log(std::string text)
{
    GtkTextBuffer* tbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(log_panel));

    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(tbuffer, &iter);

    std::string tolog = text + "\n";
    gtk_text_buffer_insert(tbuffer, &iter, tolog.c_str(), strlen(tolog.c_str()));
}

void MainWindow::start_engine_button_clicked_callback(GtkButton* btn, MainWindow* window)
{
    window->log("Audio engine started");
    window->engine->start();

    //disable device change
    gtk_widget_set_sensitive(GTK_WIDGET(window->audio_device_catalog), false);
}

void MainWindow::stop_engine_button_clicked_callback(GtkButton* btn, MainWindow* window)
{
    window->log("Audio engine stopped.");
    if(window->engine) window->engine->end();

    //enable device change
    gtk_widget_set_sensitive(GTK_WIDGET(window->audio_device_catalog), true);
}

void MainWindow::audio_device_catalog_changed(GtkComboBox* cbx, MainWindow* window)
{
    int selection = gtk_combo_box_get_active(cbx);
    window->engine->set_current_device(selection);
}

bool MainWindow::playfield_aux_darea_key_down(GtkEventControllerKey* eck, guint keyval, guint keycode, GdkModifierType state, MainWindow* window)
{
    if(keyval == GDK_KEY_Control_L)
    {
        //enable wire delete mode
        window->is_delete_key = true;

    }
    return true;
}

void MainWindow::playfield_aux_darea_key_up(GtkEventControllerKey* eck, guint keyval, guint keycode, GdkModifierType state, MainWindow* window)
{
    if(keyval == GDK_KEY_Control_L)
    {
        //reset delete bit
        //xor with itself
        window->is_delete_key ^= window->is_delete_key;

        //lets not keep anyone selected
        for(signal_path* sp : window->signal_path_list)
            sp->set_ui_selected(false);
    }
}

void MainWindow::playfield_aux_darea_gesture_click(GtkGestureClick* ges, int npresses, double x, double y, MainWindow* window)
{
    int button_number = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(ges));

    //if we have the right-button clicked
    if(button_number == 3 /*this is right click*/)
    {
        //show popover
        GdkRectangle rect = {x, y, 1, 1};
        gtk_popover_set_pointing_to(GTK_POPOVER(window->darea_popover), &rect);
        gtk_popover_popdown(GTK_POPOVER(window->darea_popover));
        gtk_popover_present(GTK_POPOVER(window->darea_popover));
        
        //window->populate_darea_popover();
        gtk_widget_show(GTK_WIDGET(window->darea_popover));
    }
}


void MainWindow::remove_signal_path(signal_path* s_path)
{
    //delete from the list
    signal_path_list.erase(std::remove(signal_path_list.begin(), signal_path_list.end(), s_path), signal_path_list.end());
}  

void MainWindow::hbar_midi_options_button_clicked(GtkButton* btn, MainWindow* window)
{
    
    //this cannot be a stack variable as these get deleted at the end of the scope, which effectively kills the dialog right as it is shownE

    midi_options_dialog* dlg = new midi_options_dialog(window->main_window, window->engine);

    dlg->show();
    //window->midi_dialog->show();
    //gtk_widget_show(GTK_WIDGET(window->midi_dialog));
}

void MainWindow::hbar_collapse_panel_button_toggled(GtkToggleButton* btn, MainWindow* window)
{
    if (gtk_toggle_button_get_active(btn) == true)
    {
        /*show*/
        gtk_button_set_icon_name(GTK_BUTTON(btn), "sidebar-collapse");
        //gtk_paned_set_position(window->main_grid, window->last_sidebar_position);~
        adw_overlay_split_view_set_show_sidebar(window->main_grid, true);
    }
    else if(gtk_toggle_button_get_active(btn) == false)
    {
        /*collapse*/
        gtk_button_set_icon_name(GTK_BUTTON(btn), "sidebar-expand");
        //window->last_sidebar_position = gtk_paned_get_position(window->main_grid);
        //gtk_paned_set_position(window->main_grid, 0);
        adw_overlay_split_view_set_show_sidebar(window->main_grid, false);
    }
}

void MainWindow::hbar_start_engine_button_toggled(GtkToggleButton* btn, MainWindow* window)
{
    //new state
    bool is_active = gtk_toggle_button_get_active(btn);

    if(is_active)
    {
        window->log("Audio engine started");
        window->engine->start();

        //disable device change
        gtk_widget_set_sensitive(GTK_WIDGET(window->audio_device_catalog), false);

        gtk_toggle_button_set_active(window->hbar_start_engine, true);
    } 
    else 
    {
        gtk_toggle_button_set_active(window->hbar_start_engine, true);
    }
}


void MainWindow::hbar_stop_engine_button_clicked(GtkButton* btn, MainWindow* window)
{
    gtk_toggle_button_set_active(window->hbar_start_engine, false);
    gtk_widget_queue_draw(GTK_WIDGET(window->hbar_start_engine));

    window->log("Audio engine stopped");
    window->engine->end();

    //disable device change
    gtk_widget_set_sensitive(GTK_WIDGET(window->audio_device_catalog), true);
}

void MainWindow::populate_darea_popover()
{
    //take the Menu and add a button for each widget
    //clear the menu

    //for each widget add a button
    std::vector<widget_manager::widget_entry>& database = wmanager->get_widget_database();

    for(widget_manager::widget_entry entry : database)
    {     
        GtkListBoxRow* row = (GtkListBoxRow*) gtk_list_box_row_new();
        GtkLabel* label = (GtkLabel*) gtk_label_new(entry.long_name.c_str());

        gtk_list_box_row_set_child(row, GTK_WIDGET(label));
        gtk_actionable_set_detailed_action_name(GTK_ACTIONABLE(row), ("app.add_widget::" + entry.name).c_str());
        gtk_list_box_row_set_activatable(row, true);
        gtk_widget_set_sensitive(GTK_WIDGET(row), true);

        gtk_widget_set_halign(GTK_WIDGET(label), GTK_ALIGN_START);
        gtk_label_set_justify(label, GTK_JUSTIFY_LEFT);

        gtk_list_box_append(darea_popover_lb, GTK_WIDGET(row));
    }
}

void MainWindow::create_app_actions()
{
    //GSimpleACtions
    //add_widget
    app_add_widget_action = (GSimpleAction*) g_simple_action_new("add_widget", G_VARIANT_TYPE_STRING);
    app_show_properties_action = (GSimpleAction*) g_simple_action_new("show_properties", G_VARIANT_TYPE_UINT64);

    g_signal_connect(app_add_widget_action, "activate", G_CALLBACK(app_action_callback), this);
    g_signal_connect(app_show_properties_action, "activate", G_CALLBACK(app_action_callback), this);

    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(app_add_widget_action));
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(app_show_properties_action));
}

void MainWindow::app_action_callback(GSimpleAction* act, GVariant* param, MainWindow* window)
{
    std::string action_name = g_action_get_name(G_ACTION(act));
    //handle correspondingly
    if(action_name == "add_widget")
    {
        //this will create widget
        std::string target = g_variant_get_string(param, NULL);
        audio_widget* new_audio = window->wmanager->create_widget(target);

        new_audio->program_context = window->program_context;
        new_audio->name = target;
        new_audio->long_name = window->wmanager->get_widget_long_name(target);

        window->playfield_add_widget(new_audio);
    }
    else if(action_name == "show_properties")
    {
        //show stack
        gtk_stack_set_visible_child_name(window->right_stack, "wprefs");
    }
}

void MainWindow::darea_popover_lv_fac_setup(GtkSignalListItemFactory* fac, GtkListItem* list_item, MainWindow* window)
{
    std::string long_name = "";

    GtkLabel* item = (GtkLabel*) gtk_label_new(long_name.c_str());
    gtk_list_item_set_child(list_item, GTK_WIDGET(item));
}

/*void MainWindow::darea_popover_lv_fac_bind(GtkSignalListItemFactory* fac, GtkListItem* object, MainWindow* window)
{
    int pos = gtk_list_item_get_position(object);
    const char* name = gtk_string_list_get_string(window->darea_popover_list_store, pos);

    std::string long_name = window->wmanager->get_widget_long_name(name);

    GtkLabel* label = (GtkLabel*) gtk_list_item_get_child(object);
    gtk_label_set_text(label, long_name.c_str());
    gtk_label_set_justify(label, GTK_JUSTIFY_LEFT);
    gtk_widget_set_hexpand(GTK_WIDGET(label), true);
}*/

void MainWindow::darea_popover_activated(GtkListBox* lb, GtkListBoxRow* row, MainWindow* window)
{
}

void MainWindow::build_widget_properties(audio_widget* widget)
{
    //remove previous preferences page
    gtk_stack_remove(right_stack, GTK_WIDGET(right_preferences));

    //readd
    right_preferences = (AdwPreferencesPage*) adw_preferences_page_new();
    gtk_stack_add_named(right_stack, GTK_WIDGET(right_preferences), "wprefs");

    //show and build
    gtk_stack_set_visible_child_name(right_stack, "wprefs");
    widget->build_properties_page(right_preferences);
}