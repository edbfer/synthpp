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

#include "settings_manager.h"
#include "widget_manager.h"

#include <gtk/gtk.h>
#include <adwaita.h>

#define PACKAGE "org.edbfer.synthpp"
#define PACKAGE_VERSION "0.0.1"

#include <bfd.h>
#include <dlfcn.h>
#include <vector>
#include <tuple>
#include <string>

enum invalid_reason
{
    normal,
    not_elf_object,
    bad_object,
    bad_checksum,
    missing_info
};

struct plugin{
    std::string name;
    std::string long_name;
    std::string description;
    std::string filepath;
    std::string checksum;
    bool isloaded;
    bool isblacklisted;
    bool isInvalid; //for files that are not elfs
    invalid_reason reason;
    void* dl_data;
};

class plugin_manager {

    public:

        plugin_manager(settings_manager* setman, widget_manager* widman);

        void load_plugins();

    private:
        //plugin database in memory
        std::vector<plugin> plugin_database;
        std::vector<plugin> candidates;

        void create_plugin_selector_dialog();
        void populate_bfd_info(plugin& p);
        bool find_symbol(bfd* cur, asymbol** symtab, int nsym, std::string symbol, unsigned long& offset);
        //bool find_symbol(asymbol** symtab, int nsym, std::string symbol, void*& retval);

        //reference to the settings manager
        settings_manager* setman;
        widget_manager* widman;

        //dialog vars
        GtkDialog* plugin_dialogs;
        AdwPreferencesPage* plugin_list;
        AdwPreferencesGroup* plugin_list_group;
        std::vector<std::pair<AdwActionRow, plugin>> list_of_rows;
};