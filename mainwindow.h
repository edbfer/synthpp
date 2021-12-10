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
#include <gtkmm/label.h>
#include <gtkmm/textview.h>
#include <gtkmm/scrolledwindow.h>

// Mainwindow class
//
class MainWindow : public Gtk::Window {

    public:

        //constructor
        MainWindow();

    protected:
        Gtk::Grid main_grid;
        Gtk::Fixed playfield;

        Gtk::Grid right_panel;
        Gtk::Label log_label;
        Gtk::TextView log_panel;
        Gtk::ScrolledWindow log_panel_scroll;

};