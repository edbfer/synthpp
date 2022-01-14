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

#include <pthread.h>
#include <algorithm>
#include <tuple>

#include <gdkmm/cairocontext.h>
#include <gdkmm/cairoutils.h>
#include <cairomm/cairomm.h>
#include <gtkmm/layoutmanager.h>
#include <gdkmm/surface.h>
#include <glibmm/main.h>
#include <gtkmm/gesturedrag.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/eventcontrollermotion.h>

MainWindow::MainWindow(){

    set_title("synthpp");

    int height = 768, width = 1024;
    set_default_size(width, height);

    //create the grid widget
    set_child(main_grid);
    main_grid.set_halign(Gtk::Align::FILL);
    main_grid.set_valign(Gtk::Align::FILL);
    main_grid.set_expand(true);

    //right panel
    main_grid.attach(right_panel, 0, 0, 1, 1);
    right_panel.set_margin(10);
    right_panel.set_halign(Gtk::Align::FILL);
    right_panel.set_valign(Gtk::Align::FILL);
    right_panel.set_vexpand(true);
    right_panel.attach(log_label, 0, 0, 1, 1);

    log_label.set_text("Log window");
    log_label.set_hexpand(true);
    log_label.set_margin(5);
    log_label.set_halign(Gtk::Align::FILL);
    log_label.set_can_focus(false);
    log_label.set_justify(Gtk::Justification::CENTER);

    //log view window
    right_panel.attach(log_panel_scroll, 0, 1, 1, 1);
    log_panel_scroll.set_expand(true);
    log_panel_scroll.set_child(log_panel);
    log_panel.set_expand(true);
    log_panel.set_wrap_mode(Gtk::WrapMode::CHAR);
    log_panel.set_can_focus(false);

    ///audio_device_catalog.set_hexpand(true);
    audio_device_catalog.set_margin(5);
    audio_device_catalog.signal_changed().connect(sigc::mem_fun(*this, &MainWindow::audio_device_catalog_changed));
    right_panel.attach(audio_device_catalog, 0, 2, 1, 1);

    start_engine_button.set_hexpand(true);
    start_engine_button.set_label("Start Audio Engine");
    start_engine_button.set_margin(5);
    start_engine_button.signal_clicked().connect(sigc::mem_fun(*this, &start_engine_button_clicked_callback));

    stop_engine_button.set_hexpand(true);
    stop_engine_button.set_label("Stop Audio Engine");
    stop_engine_button.set_margin(5);
    stop_engine_button.signal_clicked().connect(sigc::mem_fun(*this, &stop_engine_button_clicked_callback));

    right_panel.attach(start_engine_button, 0, 3, 1, 1);
    right_panel.attach(stop_engine_button, 0, 4, 1, 1);

    widget_catalog.set_hexpand(true);
    widget_catalog.set_margin(5);
    widget_catalog.insert(0, "debug", "Debug widget");
    widget_catalog.insert(1, "delay", "Slap-Back Delay");
    widget_catalog.insert(2, "fdelay", "Feedback Delay");
    widget_catalog.insert(3, "c4", "Counter: 4 bits");
    widget_catalog.insert(4, "c8", "Counter: 8 bits");
    widget_catalog.insert(5, "c16", "Counter: 16 bits");
    widget_catalog.insert(6, "probe", "Probe widget");
    right_panel.attach(widget_catalog, 0, 5, 1, 1);

    test_button.set_hexpand(true);
    test_button.set_label("Add");
    test_button.set_margin(5);
    test_button.signal_clicked().connect(sigc::mem_fun(*this, &test_button_clicked_callback));
    right_panel.attach(test_button, 0, 6, 1, 1);

    //place the fixed
    playfield.set_expand(true);
    playfield.set_halign(Gtk::Align::FILL);
    playfield.set_valign(Gtk::Align::FILL);
    playfield.set_margin(10);
    playfield_scroll.set_expand(true);
    playfield_scroll.set_child(playfield);
    playfield_scroll.set_halign(Gtk::Align::FILL);
    playfield_scroll.set_valign(Gtk::Align::FILL);
    playfield_scroll.signal_edge_reached().connect(sigc::mem_fun(*this, &MainWindow::scrolled_edge_reached));
    main_grid.attach(playfield_scroll, 1, 0, 7, 1);

    //drawing area
    playfield_aux_darea.set_draw_func(sigc::mem_fun(*this, &MainWindow::playfield_aux_darea_draw));
    playfield_aux_darea.set_size_request(width, height);
    //get size of fixed
    playfield.put(playfield_aux_darea, 0, 0);

    //show eevent
    signal_show().connect(sigc::mem_fun(*this, &MainWindow::mainwindow_show_callback));
    //trigger redraws
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &MainWindow::playfield_signal_redraw), 16.7);

    //attach drawing area mouse controller
    Glib::RefPtr<Gtk::GestureDrag> darea_gesture_drag = Gtk::GestureDrag::create();
    playfield_aux_darea.add_controller(darea_gesture_drag);

    Glib::RefPtr<Gtk::EventControllerMotion> darea_mouse_motion = Gtk::EventControllerMotion::create();
    playfield_aux_darea.add_controller(darea_mouse_motion);

    darea_mouse_motion->signal_motion().connect(sigc::mem_fun(*this, &MainWindow::playfield_aux_darea_motion));
    darea_gesture_drag->signal_drag_begin().connect(sigc::mem_fun(*this, &MainWindow::playfield_aux_darea_begin_grab));
    darea_gesture_drag->signal_drag_update().connect(sigc::mem_fun(*this, &MainWindow::playfield_aux_darea_update_grab));
    darea_gesture_drag->signal_drag_end().connect(sigc::mem_fun(*this, &MainWindow::playfield_aux_darea_end_grab));

    //initialize vars
    can_create_path = false;
    darea_mouse_grabbed = false;

    logger_ref.log = sigc::mem_fun(*this, &MainWindow::log);
    logger_ref.put_widget = sigc::mem_fun(*this, &MainWindow::playfield_add_widget);
    logger_ref.remove_widget = sigc::mem_fun(*this, &MainWindow::playfield_remove_widget);
}

void MainWindow::playfield_add_widget(audio_widget* awidget)
{
    playfield_widget_list.push_back(awidget);

    playfield.put(*awidget, 300, 300);

    awidget->show();

    playfield_aux_darea.queue_draw();
}

void MainWindow::playfield_remove_widget(audio_widget* awidget)
{
    playfield_widget_list.erase(std::remove(playfield_widget_list.begin(), playfield_widget_list.end(), awidget), playfield_widget_list.end());

    playfield.remove(*awidget);
}

void MainWindow::test_button_clicked_callback()
{
    audio_widget* to_add;

    std::string selection = widget_catalog.get_active_id();

    if (selection == "debug")
    {
        to_add = new debug_widget(300, 300);
    }
    else if(selection == "delay")
    {
        to_add = new delay_widget();
    }
    else if(selection == "fdelay")
    {
        to_add = new feedback_delay_widget();
    }
    else if(selection == "c4")
    {
        to_add = new counter_widget(4);
    }
    else if(selection == "c8")
    {
        to_add = new counter_widget(8);
    }
    else if(selection == "c16")
    {
        to_add = new counter_widget(16);
    }
       else if(selection == "probe")
    {
        to_add = new probe_widget();
    }

    playfield_add_widget(to_add);
    
    //redraw
    playfield_aux_darea.queue_draw();
}

void MainWindow::playfield_aux_darea_draw(Glib::RefPtr<Cairo::Context> cairo_context, int width, int height)
{
    int peg_radius = 6;
    int bias = 2;

    cairo_context->save();

    cairo_context->set_source_rgb(0.25/4., 0.125/4., 0.0);
    cairo_context->rectangle(0, 0, width, height);
    cairo_context->fill_preserve();

    cairo_context->restore();
    cairo_context->begin_new_path();

    for (audio_widget* aw : playfield_widget_list)
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
                cairo_context->set_source_rgb(1.0, 0.5, 0.0);
            }
            else if(p->is_grabbed())
            {
                cairo_context->set_source_rgb(1.0, 0.0, 0.0);
            }
            else
            {
                if(p->get_direction() == port::port_type::OUTPUT)
                    cairo_context->set_source_rgb(0.0, 1.0, 0.0);
                else if(p->get_direction() == port::port_type::INPUT)
                    cairo_context->set_source_rgb(0.0, 1.0, 0.0);
            }
            cairo_context->arc(x, y, peg_radius, 0, 2.*M_PI);
            cairo_context->fill();

            if(p->is_grabbed())
            {
                cairo_context->move_to(x, y);
                cairo_context->line_to(darea_cur_mouse_x, darea_cur_mouse_y);
                cairo_context->set_line_width(4);

                //do we have a path?
                if(can_create_path)
                {
                    //add tooltip

                    int mid_x = (x + darea_cur_mouse_x)/2;
                    int mid_y = (y + darea_cur_mouse_y)/2;
                    cairo_context->move_to(mid_x, mid_y);
                    cairo_context->set_source_rgb(1.0, 1.0, 1.0);
                    cairo_context->show_text("Create path: " + candidate_path->get_path_name());

                    cairo_context->set_source_rgb(0.0, 0.0, 1.0);
                }
                cairo_context->stroke();
            }
        }
    }

    //now draw the paths
    for(signal_path* s_path : signal_path_list)
    {
        int source_x, source_y;
        int dest_x, dest_y;

        s_path->get_source_port()->get_position_indarea(source_x, source_y);
        s_path->get_destination_port()->get_position_indarea(dest_x, dest_y);

        source_x -= peg_radius; //input
        dest_x += peg_radius; //output

        source_y += peg_radius + bias;
        dest_y += peg_radius + bias;

        //label
        int mid_x = (source_x + dest_x)/2;
        int mid_y = (source_y + dest_y)/2;
        cairo_context->move_to(mid_x, mid_y);
        cairo_context->show_text("path_" + s_path->get_path_name());

        cairo_context->set_source_rgb(0.0, 1.0, 0.0);

        cairo_context->move_to(source_x, source_y);
        cairo_context->line_to(dest_x, dest_y);      
        cairo_context->set_line_width(4);
        cairo_context->stroke();  
    }
}

void MainWindow::gdk_surface_layout_callback(int width, int height)
{
    //std::cout << "New size: " << width << ", " << height << std::endl;
    //increase the darea size, if needed
    int cur_width = playfield_aux_darea.get_allocation().get_width();
    int cur_height = playfield_aux_darea.get_allocation().get_height();

    int new_width = std::max(cur_width, width);
    int new_height = std::max(cur_height, height);

    playfield_aux_darea.set_size_request(new_width, new_height);
}

void MainWindow::mainwindow_show_callback()
{
    //attach to window size change event
    auto gdk_surface = get_surface();
    gdk_surface->signal_layout().connect(sigc::mem_fun(*this, &MainWindow::gdk_surface_layout_callback));

    engine = new audio_engine(&playfield_widget_list, &signal_path_list, logger_ref);

    //clip text
    int width = audio_device_catalog.get_allocation().get_width();
    Gtk::CellRendererText* cell_renderer = (Gtk::CellRendererText*) audio_device_catalog.get_first_cell();
    cell_renderer->property_width_chars() = 1;
    cell_renderer->property_ellipsize() = Pango::EllipsizeMode::END;
    cell_renderer->set_fixed_size(width, -1);

    //populate device list
    int position = 0;
    for(std::pair info_pair : *(engine->get_device_vector()))
    {
        audio_device_catalog.insert(position++, std::string(info_pair.second->name) + " (in: " + std::to_string(info_pair.second->maxInputChannels) + ", out: " + std::to_string(info_pair.second->maxOutputChannels) + ")");
    }
}

void MainWindow::scrolled_edge_reached(Gtk::PositionType pos_type)
{
    int cur_width = playfield_aux_darea.get_allocation().get_width();
    int cur_height = playfield_aux_darea.get_allocation().get_height();

    int playfield_width = playfield.get_allocation().get_width();
    int playfield_height = playfield.get_allocation().get_height();

    int new_w = std::max(cur_width, playfield_width);
    int new_h = std::max(cur_height, playfield_height);

    playfield_aux_darea.set_size_request(new_w, new_h);
}

void MainWindow::playfield_trigger_redraw()
{
    do_redraw = true;
    //return true;
}

bool MainWindow::playfield_signal_redraw()
{
    playfield_aux_darea.queue_draw();

    //also, ask if widgets want to redraw
    for(audio_widget* aw : playfield_widget_list)
    {
        aw->process_ui();
    }

    return true;
}

void MainWindow::playfield_aux_darea_begin_grab(double x, double y)
{
    //look if one of the pegs is near, if it is then mark it
    if(darea_mouse_grabbed == false)
    {
        //check if we are not grabbed already
        for (audio_widget* aw : playfield_widget_list)
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
                    starting_port = p;
                    darea_mouse_grabbed = true;
                    darea_start_mouse_x = darea_cur_mouse_x = x;
                    darea_start_mouse_y = darea_cur_mouse_y = y;

                    //create a new path
                    can_create_path = false;
                    candidate_path = new signal_path;
                }
            }
        }
    }
}

void MainWindow::playfield_aux_darea_update_grab(double offset_x, double offset_y)
{
    darea_cur_mouse_x = darea_start_mouse_x + offset_x;
    darea_cur_mouse_y = darea_start_mouse_y + offset_y;

    //clear the general connection creation state
    can_create_path = false;

    //are we near a connection?
    for(audio_widget *aw : playfield_widget_list)
    {
        int peg_radius = 6;
        int bias = 2;

        for(port *p : *(aw->get_port_list()))
        {
            int p_x, p_y;
            p->get_position_indarea(p_x, p_y);

            int peg_x = p_x + ((p->get_direction() == port::port_type::OUTPUT) ? peg_radius : -peg_radius);
            int peg_y = p_y + peg_radius + bias;

            double distance = hypot(peg_x - darea_cur_mouse_x, peg_y - darea_cur_mouse_y);
            
            if(distance < peg_radius)
            {
                //we are near another peg
                //can we attach?
                signal_path* tentative = this->candidate_path;

                if(starting_port->get_direction() == port::port_type::INPUT && p->get_direction() == port::port_type::OUTPUT)
                {
                    tentative->set_source_port(p);
                    tentative->set_destination_port(starting_port);
                    can_create_path = true;
                }
                else if(starting_port->get_direction() == port::port_type::OUTPUT && p->get_direction() == port::port_type::INPUT)
                {
                    tentative->set_source_port(starting_port);
                    tentative->set_destination_port(p);
                    can_create_path = true;
                }
            }
        }
    }

    //playfield_aux_darea.queue_draw();
}

void MainWindow::playfield_aux_darea_end_grab(double x, double y)
{
    if(darea_mouse_grabbed)
    {
        for(audio_widget* aw : playfield_widget_list)
        {
            std::vector<port*> p_list = *(aw->get_port_list());
            for(port* p : p_list)
            {
                p->set_grabbed(false);
            }
        }

        darea_mouse_grabbed = false;
        //darea_mouse_x = 0;
        //darea_mouse_y = 0;

        //can we create path?
        if(can_create_path)
            signal_path_list.push_back(candidate_path);
        else
            delete candidate_path;

        //playfield_aux_darea.queue_draw();
    }
}

void MainWindow::playfield_aux_darea_motion(double x, double y)
{
    //look if one of the pegs is near, if it is then mark it
    for (audio_widget* aw : playfield_widget_list)
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

void MainWindow::log(std::string text)
{
    Glib::RefPtr<Gtk::TextBuffer> tbuffer = log_panel.get_buffer();
    Gtk::TextBuffer::iterator iter = tbuffer->end();

    tbuffer->insert(iter, text + "\n");
}

void MainWindow::start_engine_button_clicked_callback()
{
    log("Audio engine started @ 5000Hz");
    engine->start();

    //disable device change
    audio_device_catalog.set_sensitive(false);
}

void MainWindow::stop_engine_button_clicked_callback()
{
    log("Audio engine stopped.");
    if(engine) engine->end();

    //enable device change
    audio_device_catalog.set_sensitive(true);
}

void MainWindow::audio_device_catalog_changed()
{
    int selection = audio_device_catalog.get_active_row_number();
    engine->set_current_device(selection);
}