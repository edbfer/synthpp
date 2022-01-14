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

#include <iostream>
#include <fstream>

//Circular Buffer
#include <boost/circular_buffer.hpp>

//windows
#include <gtkmm/application.h>

//main UI
#include "mainwindow.h"

using namespace std;

Glib::RefPtr<Gtk::Application> gtkmm_application;
int done = 0;

int main(int argc, char ** argv)
{
    gtkmm_application = Gtk::Application::create("org.edbfer.synthpp");

    gtkmm_application->make_window_and_run<MainWindow>(argc, argv);

    return 0;
}
