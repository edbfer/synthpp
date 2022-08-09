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

#pragma once

#include <gtk/gtk.h>

#include <map>
#include <string>
#include <filesystem>

class settings_manager
{
    public:

        //loads from the file in the user directory
        void load_settings();

        //returns the work directory
        std::string get_work_directory();

        //get_keys
        std::string get_key(std::string key, std::string default_val);

    private:

        //where settings are stored in memory
        //all settings are loaded as strings, and appropriate conversions are done via the get_key function
        std::map<std::string, std::string> settings_database;

        //work directory
        std::filesystem::path work_dir;

        //handler for the first_run dialog
        static void first_run_dialog_response(GtkDialog* self, int response);
};