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

MainWindow::MainWindow(){

    set_title("synthpp");
    set_default_size(1024, 768);

    //create the grid widget
    set_child(main_grid);
    main_grid.set_halign(Gtk::Align::FILL);
    main_grid.set_valign(Gtk::Align::FILL);
    main_grid.set_expand(true);

    //place the fixed
    main_grid.attach(playfield, 1, 0, 7, 1);
    playfield.set_expand(true);
    
    //right panel
    main_grid.attach(right_panel, 0, 0, 1, 1);
    right_panel.set_halign(Gtk::Align::FILL);
    right_panel.set_valign(Gtk::Align::FILL);
    right_panel.set_vexpand(true);
    right_panel.attach(log_label, 0, 0, 1, 1);

    log_label.set_text("Log window");
    log_label.set_hexpand(true);
    log_label.set_halign(Gtk::Align::FILL);
    log_label.set_justify(Gtk::Justification::CENTER);

    //log view window
    right_panel.attach(log_panel_scroll, 0, 1, 1, 6);
    log_panel_scroll.set_expand(true);
    log_panel_scroll.set_child(log_panel);
    log_panel.set_expand(true);
    log_panel.set_wrap_mode(Gtk::WrapMode::CHAR);
}