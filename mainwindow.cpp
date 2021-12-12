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

#include <gdkmm/cairocontext.h>
#include <gdkmm/cairoutils.h>
#include <cairomm/cairomm.h>

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
    playfield_scroll.signal_edge_reached().connect(sigc::mem_fun(*this, MainWindow::))
    main_grid.attach(playfield_scroll, 1, 0, 7, 1);

    //drawing area
    playfield_aux_darea.set_draw_func(sigc::mem_fun(*this, &MainWindow::playfield_aux_darea_draw));
    playfield_aux_darea.set_size_request(width, height);
    //get size of fixed
    playfield.put(playfield_aux_darea, 0, 0);

}

void MainWindow::playfield_add_widget(audio_widget& awidget)
{
    playfield_widget_list.push_back(&awidget);

    playfield.put(awidget, 100, 100);
}

void MainWindow::test_button_clicked_callback()
{
    audio_widget* awtest = new audio_widget;
    playfield_add_widget(*awtest);
}

void MainWindow::playfield_aux_darea_draw(Glib::RefPtr<Cairo::Context> cairo_context, int width, int height)
{
    cairo_context->set_source_rgb(1.0, 0.5, 0.0);
    cairo_context->rectangle(0, 0, width, height);
    cairo_context->fill_preserve();
}
