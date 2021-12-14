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
#include "port.h"

#include <pthread.h>

#include <gdkmm/cairocontext.h>
#include <gdkmm/cairoutils.h>
#include <cairomm/cairomm.h>
#include <gtkmm/layoutmanager.h>
#include <gdkmm/surface.h>
#include <glibmm/main.h>
#include <gtkmm/gesturedrag.h>
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
    log_label.set_justify(Gtk::Justification::CENTER);

    //log view window
    right_panel.attach(log_panel_scroll, 0, 1, 1, 1);
    log_panel_scroll.set_expand(true);
    log_panel_scroll.set_child(log_panel);
    log_panel.set_expand(true);
    log_panel.set_wrap_mode(Gtk::WrapMode::CHAR);

    test_button.set_hexpand(true);
    test_button.set_label("TEST!!!");
    test_button.set_margin(5);
    test_button.signal_clicked().connect(sigc::mem_fun(*this, &test_button_clicked_callback));
    right_panel.attach(test_button, 0, 2, 1, 1);

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
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &MainWindow::playfield_trigger_redraw), 10);

    //attach drawing area mouse controller
    Glib::RefPtr<Gtk::GestureDrag> darea_gesture_drag = Gtk::GestureDrag::create();
    playfield_aux_darea.add_controller(darea_gesture_drag);

    Glib::RefPtr<Gtk::EventControllerMotion> darea_mouse_motion = Gtk::EventControllerMotion::create();
    playfield_aux_darea.add_controller(darea_mouse_motion);

    darea_mouse_motion->signal_motion().connect(sigc::mem_fun(*this, &MainWindow::playfield_aux_darea_motion));
    darea_gesture_drag->signal_drag_begin().connect(sigc::mem_fun(*this, &MainWindow::playfield_aux_darea_begin_grab));
    darea_gesture_drag->signal_drag_update().connect(sigc::mem_fun(*this, &MainWindow::playfield_aux_darea_update_grab));
    darea_gesture_drag->signal_drag_end().connect(sigc::mem_fun(*this, &MainWindow::playfield_aux_darea_end_grab));
}

void MainWindow::playfield_add_widget(audio_widget& awidget)
{
    playfield_widget_list.push_back(&awidget);

    playfield.put(awidget, 100, 100);
}

void MainWindow::test_button_clicked_callback()
{
    debug_widget* dwtest = new debug_widget;
    playfield_add_widget(*dwtest);
    
    //redraw
    playfield_aux_darea.queue_draw();
}

void MainWindow::playfield_aux_darea_draw(Glib::RefPtr<Cairo::Context> cairo_context, int width, int height)
{
    cairo_context->save();

    cairo_context->set_source_rgb(0.25/4., 0.125/4., 0.0);
    cairo_context->rectangle(0, 0, width, height);
    cairo_context->fill_preserve();

    cairo_context->restore();
    cairo_context->begin_new_path();

    for (audio_widget* aw : playfield_widget_list)
    {
        int aw_x = aw->get_allocation().get_x();
        int aw_y = aw->get_allocation().get_y();

        int fixed_x, fixed_y;
        aw->get_underlaying_fixed_position(fixed_x, fixed_y);

        int aw_w = aw->get_allocation().get_width();
        int aw_h = aw->get_allocation().get_height();

        int peg_radius = 6;
        int bias = 2;

        std::vector<port*> p_list = *(aw->get_port_list());
        for(port* p : p_list)
        {
            int p_x, p_y;
            p->get_position_inwidget(p_x, p_y);

            int x;
            if(p->get_direction() == port::port_type::OUTPUT)
            {
                x = aw_x + aw_w;
            }
            int y = p_y + aw_y + fixed_y + peg_radius + bias;

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
                cairo_context->set_source_rgb(0.0, 1.0, 0.0);
            }

            double x_coord = double(x)/double(width);
            double y_coord = double(y)/double(height);
            cairo_context->arc(x, y, peg_radius, 0, 2.*M_PI);
            cairo_context->fill();

            if(p->is_grabbed())
            {
                cairo_context->move_to(x, y);
                cairo_context->line_to(darea_cur_mouse_x, darea_cur_mouse_y);
                cairo_context->set_line_width(4);
                cairo_context->stroke();
            }
        }
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

bool MainWindow::playfield_trigger_redraw()
{
    playfield_aux_darea.queue_draw();
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
            int aw_x = aw->get_allocation().get_x();
            int aw_y = aw->get_allocation().get_y();

            int fixed_x, fixed_y;
            aw->get_underlaying_fixed_position(fixed_x, fixed_y);

            int aw_w = aw->get_allocation().get_width();
            int aw_h = aw->get_allocation().get_height();

            int peg_radius = 6;
            int bias = 2;

            std::vector<port*> p_list = *(aw->get_port_list());
            for(port* p : p_list)
            {
                //por def, we set_hovered(false)
                p->set_grabbed(false);

                int p_x, p_y;
                p->get_position_inwidget(p_x, p_y);

                int peg_x;
                if(p->get_direction() == port::port_type::OUTPUT)
                {
                    peg_x = aw_x + aw_w;
                }
                int peg_y = p_y + aw_y + fixed_y + peg_radius + bias;

                double distance = hypot(peg_x - x, peg_y - y);
                if (distance < peg_radius)
                {
                    //we are in, marca como occupied
                    p->set_grabbed(true);
                    darea_mouse_grabbed = true;
                    darea_start_mouse_x = x;
                    darea_start_mouse_y = y;
                }
            }
        }
    }
}

void MainWindow::playfield_aux_darea_update_grab(double offset_x, double offset_y)
{
    darea_cur_mouse_x = darea_start_mouse_x + offset_x;
    darea_cur_mouse_y = darea_start_mouse_y + offset_y;
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
    }
}

void MainWindow::playfield_aux_darea_motion(double x, double y)
{
    //look if one of the pegs is near, if it is then mark it
    for (audio_widget* aw : playfield_widget_list)
    {
        int aw_x = aw->get_allocation().get_x();
        int aw_y = aw->get_allocation().get_y();

        int fixed_x, fixed_y;
        aw->get_underlaying_fixed_position(fixed_x, fixed_y);

        int aw_w = aw->get_allocation().get_width();
        int aw_h = aw->get_allocation().get_height();

        int peg_radius = 6;
        int bias = 2;

        std::vector<port*> p_list = *(aw->get_port_list());
        for(port* p : p_list)
        {
            //por def, we set_hovered(false)
            p->set_hovered(false);

            int p_x, p_y;
            p->get_position_inwidget(p_x, p_y);

            int peg_x;
            if(p->get_direction() == port::port_type::OUTPUT)
            {
                peg_x = aw_x + aw_w;
            }
            int peg_y = p_y + aw_y + fixed_y + peg_radius + bias;

            double distance = hypot(peg_x - x, peg_y - y);
            if (distance < peg_radius)
            {
                //we are in, marca como occupied
                p->set_hovered(true);
            }
        }
    }
}