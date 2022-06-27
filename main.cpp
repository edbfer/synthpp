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
#include <glib.h>
#include <adwaita.h>

//main UI
#include "mainwindow.h"

//load plugins
//dynamic lib stuff
#include <dlfcn.h>

using namespace std;

int done = 0;

int main(int argc, char ** argv)
{
    AdwApplication* application = NULL;

    MainWindow window;

    //g_log_set_always_fatal(G_LOG_LEVEL_CRITICAL|G_LOG_LEVEL_WARNING);
    //g_setenv("G_DEBUG", "fatal-criticals", true);

    application = adw_application_new("org.edbfer.syntpp", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(application, "activate", G_CALLBACK(MainWindow::create_layout), &window);

    return g_application_run(G_APPLICATION(application), argc, argv);   
}
