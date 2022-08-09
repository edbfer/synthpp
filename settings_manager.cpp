// Copyright (C) 2022 Eduardo Ferreira
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
//check if the settings file exists
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <gtk/gtk.h>

#include "settings_manager.h"

#include <filesystem>
#include <fstream>

void settings_manager::load_settings()
{
    //this will load the settings from the user's home directory
    //use $HOME to get the home dir
    const char* home_dir;

    if((home_dir = getenv("HOME")) == NULL)
    {
        //use getpwuid
        home_dir = getpwuid(getuid())->pw_dir;
    }

    //create filesystem std lib context
    std::filesystem::path settings_path = home_dir;
    settings_path /= "Synthpp";

    work_dir = settings_path;

    //does it exist?
    //if not, this is probably a first run
    bool first_run = false;
    if(!std::filesystem::exists(settings_path))
    {
        //make it
        std::filesystem::create_directory(settings_path);
        first_run = true;
    }

    std::filesystem::path settings_file = settings_path / "app_settings.dat";

    if(!std::filesystem::exists(settings_file))
    {
        first_run = true;
        //this creates the file
        std::ofstream create_file(settings_file);
        create_file.close();
    }

    std::ifstream settings_stream(settings_file);

    if(!first_run)
    {
        //load the settings line by line
        std::string line;
        while(std::getline(settings_stream, line))
        {
            //check if line starts with # to ignore
            if(line.at(0) == '#')
                continue;

            //parse string
            std::string key = line.substr(0, line.find(':'));
            line.erase(0, line.find(':') + 1);
            std::string val = line;

            settings_database[key] = line;
        }
    }
    else
    {
        //create a default settings file
        //warn the user
        GtkDialog* first_run_message = (GtkDialog*) gtk_message_dialog_new(NULL, GTK_DIALOG_USE_HEADER_BAR, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "A default working directory was created in %s", settings_path.c_str());
        gtk_window_set_title(GTK_WINDOW(first_run_message), "First run");

        gtk_widget_show(GTK_WIDGET(first_run_message));
        g_signal_connect(first_run_message, "response", G_CALLBACK(settings_manager::first_run_dialog_response), NULL);
    }
    
}

void settings_manager::first_run_dialog_response(GtkDialog* self, int response)
{
    gtk_window_destroy(GTK_WINDOW(self));
}

std::string settings_manager::get_work_directory()
{
    return work_dir;
}


std::string settings_manager::get_key(std::string key, std::string default_val)
{
    //is the key set?
    if(settings_database.count(key) == 1)
    {
        //it is set
        return settings_database.at(key);
    }
    else
    {
        //it is not set
        //set it using default value
        settings_database[key] = default_val;
        return default_val;
    }
    
}
