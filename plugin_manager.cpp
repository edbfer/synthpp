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

#include "plugin_manager.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include <glib.h>

plugin_manager::plugin_manager(settings_manager* setman, widget_manager* widman) : setman(setman), widman(widman)
{
    //initialize bfd
    //this is for parsing elf files without actually loading them in code memory
    bfd_init();
}

void plugin_manager::load_plugins()
{
    //does a plugin folder exist already?
    std::filesystem::path work_dir = setman->get_work_directory();
    std::filesystem::path plugin_folder = setman->get_key("plugin_manager.plugin_folder", work_dir / "Plugins");

    if(!std::filesystem::exists(plugin_folder))
    {
        std::filesystem::create_directory(plugin_folder);
    }

    //read plugin database
    if(std::filesystem::exists(work_dir / "plugin_database.dat"))
    {
        std::ifstream database_file(work_dir / "plugin_database.dat");
        //parse plugin database from file
        while(!database_file.eof())
        {
            //database is ordered as
            //filepath    checksum    blacklist
            plugin p;
            database_file >> p.filepath >> p.checksum >> p.isblacklisted;
            plugin_database.push_back(p);
        }
    }

    //now that we have the known plugin database, we will parse all files in the plugin folder. Plugins that are already known and not blacklisted are automatically loaded
    std::vector<plugin> candidate_list;
    for(auto& file : std::filesystem::directory_iterator(plugin_folder))
    {
        //is a file?
        if(!file.is_regular_file())
            continue;

        //check extension
        std::filesystem::path file_path = file;
        if(!(file_path.extension() == ".sppp"))
            continue;

        //this is a candidate
        plugin p;
        p.filepath = file_path;
        candidate_list.push_back(p);
    }

    //now check the candidates against the database
    //if plugin not found in the database, we will need the dialog
    for(auto it = candidate_list.begin(); it < candidate_list.end(); it++)
    {
        //get path and check if there is in the database
        std::filesystem::path plugin_path = it->filepath;
        int nbytes = std::filesystem::file_size(plugin_path);

        bool ispresent = false;
        int index = -1;
        for(auto it = plugin_database.begin(); it < plugin_database.end(); it++)
        {
            //we have the same file, keep register to check info
            if(it->filepath == it->filepath)
            {
                ispresent = true;
                index = it - plugin_database.begin();
            }
        }
        if(!ispresent) continue; //we will handle it in the dialog

        //get database entry
        plugin& database_entry = plugin_database.at(index);

        //allocate buffer for checksum
        char* buffer = new char[nbytes];

        //read bytes
        std::ifstream plugin_file(plugin_path, std::ios::in | std::ios::binary);
        plugin_file.read(buffer, nbytes);

        //calculate checksum and cross-check
        GChecksum* checksum = g_checksum_new(G_CHECKSUM_SHA512);
        size_t dig_size = nbytes;
        g_checksum_get_digest(checksum, (unsigned char*) buffer, &dig_size);
        std::string digest = g_checksum_get_string(checksum);

        delete buffer;

        //check if digest matches, otherwise we will leave it for the dialog
        if(digest != database_entry.checksum)
        {
            it->isInvalid = true;
            it->reason = bad_checksum;
            continue;
        }

        //we keep going, things match
        //load file with dlopen and get the symbols
        //is blacklisted?
        if(database_entry.isblacklisted)
        {
            //we do nothing, and just delete from the candidates
            candidate_list.erase(it);
        }
        database_entry.dl_data = dlopen(it->filepath.c_str(), RTLD_NOW | RTLD_GLOBAL);
        database_entry.name = *((std::string*) dlsym(database_entry.dl_data, "plugin_name"));
        database_entry.long_name = *((std::string*) dlsym(database_entry.dl_data, "plugin_long_name"));
        database_entry.description = *((std::string*) dlsym(database_entry.dl_data, "plugin_description"));
        database_entry.isloaded = true;
        database_entry.isInvalid = false;
        database_entry.reason = normal;

        //register widget
        audio_widget* (*cfun)() = (audio_widget*(*)()) dlsym(database_entry.dl_data, "create_widget_instance");
        widman->register_widget(database_entry.name, database_entry.long_name, database_entry.description, cfun);

        //we can remove it from the candidates
        candidate_list.erase(it);
    }

    //do we have plugins remaining?
    //if(candidate_list.size() == 0) return;

    candidates = candidate_list;
    //we need the dialog
    create_plugin_selector_dialog();

    gtk_widget_show(GTK_WIDGET(plugin_dialogs));

}

void plugin_manager::create_plugin_selector_dialog()
{
    GtkDialogFlags flags = GTK_DIALOG_USE_HEADER_BAR | GTK_DIALOG_MODAL;

    std::string dialog_title = std::to_string(candidates.size()) + " new plugin" + ((candidates.size() == 1) ? "" : "s") + " found";

    plugin_dialogs = (GtkDialog*) gtk_dialog_new_with_buttons(dialog_title.c_str(), NULL, flags, "Apply", GTK_RESPONSE_APPLY, "Skip", GTK_RESPONSE_CANCEL, NULL);

    plugin_list = (AdwPreferencesPage*) adw_preferences_page_new();
    gtk_widget_set_halign(GTK_WIDGET(plugin_list), GTK_ALIGN_FILL);
    gtk_widget_set_valign(GTK_WIDGET(plugin_list), GTK_ALIGN_FILL);
    gtk_window_set_child(GTK_WINDOW(plugin_dialogs), GTK_WIDGET(plugin_list));

    plugin_list_group = (AdwPreferencesGroup*) adw_preferences_group_new();
    adw_preferences_group_set_title(plugin_list_group, "Select plugins to load");
    adw_preferences_group_set_description(plugin_list_group, "Only load plugins from trustworthy sources. Make sure no unwanted plugins are loaded.");
    adw_preferences_page_add(plugin_list, plugin_list_group);

    for(plugin& p : candidates)
    {
        //we add a line for each new plugin
        //fill in info from bfd
        populate_bfd_info(p);
        AdwActionRow* row = (AdwActionRow*) adw_action_row_new();
        
        std::string display_long_name;
        std::string description;
        if(p.isInvalid)
        {
            switch(p.reason)
            {
                case invalid_reason::not_elf_object:
                case invalid_reason::bad_object:
                {
                    display_long_name = "Error: file is not a Synthpp plugin.";
                    description = p.filepath;
                    break;
                }

                case invalid_reason::missing_info:
                {
                    if(p.long_name == "")
                    {
                        display_long_name = "Missing name";
                    }
                    if(p.description == "")
                    {
                        description = "Missing description";
                    }
                    break;
                }
            }
        }
        else
        {
            display_long_name = p.long_name;
            description = p.description;
        }

        adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), display_long_name.c_str());
        adw_action_row_set_subtitle(row, description.c_str());

        //is invalid? use icon
        if(p.isInvalid && (p.reason != invalid_reason::missing_info))       
            adw_action_row_set_icon_name(row, "gtk-dialog-error");
        else
        {
            if(p.isInvalid && (p.reason == invalid_reason::missing_info))              
                adw_action_row_set_icon_name(row, "gtk-dialog-warning");

            //can add the toggle button
            GtkSwitch* sw = (GtkSwitch*) gtk_switch_new();
            gtk_widget_set_vexpand(GTK_WIDGET(sw), false);
            gtk_widget_set_valign(GTK_WIDGET(sw), GTK_ALIGN_CENTER);
            adw_action_row_add_suffix(row, GTK_WIDGET(sw));
        }

        adw_preferences_group_add(plugin_list_group, GTK_WIDGET(row));
    }
}

void plugin_manager::populate_bfd_info(plugin& p)
{
    bfd* cur = bfd_openr(p.filepath.c_str(), NULL);

    //check if elf
    bfd_format* fmt;
    if(!bfd_check_format(cur, bfd_object))
    {
        p.isInvalid = true;
        p.reason = invalid_reason::not_elf_object;
        return;
    }

    //else, we check the magic
    //need the symtab
    int symtab_nbytes = bfd_get_symtab_upper_bound(cur);
    asymbol** symtab = (asymbol**) new char[symtab_nbytes];
    int nsym = bfd_canonicalize_symtab(cur, symtab);
    if(symtab_nbytes == 0 || nsym == 0)
    {
        //there are no symbols
        delete symtab;
        p.isInvalid = true;
        p.reason = invalid_reason::bad_object;
        return;
    }

    //find the symbols
    int nerr = 0; //only if nerr == 0 we have all the symbols

    unsigned long offset = 0;
    std::ifstream pl_stream(p.filepath);
    char* buffer = new char[std::filesystem::file_size(p.filepath)];
    pl_stream.read(buffer, std::filesystem::file_size(p.filepath));

    std::string magic = "";

    bool havemagic = find_symbol(cur, symtab, nsym, "magic_string", offset);
    unsigned long at_ptr = *((unsigned long*) (buffer + offset));
    if (offset != 0) {magic = std::string(buffer + at_ptr);}
    
    bool magicgood = (magic == "SynthPP-Plugin-v1");
    if(!havemagic || !magicgood)
    {
        //not a plugin
        delete symtab;
        p.isInvalid = true;
        p.reason = invalid_reason::bad_object;
        return;
    }

    if(!find_symbol(cur, symtab, nsym, "plugin_name", offset)) nerr++;
    else {at_ptr = *((unsigned long*) (buffer + offset)); p.name = std::string(buffer + at_ptr);}

    if(!find_symbol(cur, symtab, nsym, "plugin_long_name", offset)) nerr++;
    else {at_ptr = *((unsigned long*) (buffer + offset)); p.long_name = std::string(buffer + at_ptr);}

    if(!find_symbol(cur, symtab, nsym, "plugin_description", offset)) nerr++;
    else {at_ptr = *((unsigned long*) (buffer + offset)); p.description = std::string(buffer + at_ptr);}
    
    unsigned long pfunc = 0;
    if(!find_symbol(cur, symtab, nsym, "create_widget_instance", pfunc))
    {
        //no code
        delete symtab;
        p.isInvalid = true;
        p.reason = invalid_reason::bad_object;
        return;
    }

    if(nerr != 0 || p.reason == invalid_reason::bad_checksum) p.isInvalid = true;
    if(nerr != 0 && p.reason != invalid_reason::bad_checksum) p.reason = invalid_reason::missing_info;
    if(nerr == 0 && p.reason != invalid_reason::bad_checksum)
    {
        p.reason = normal;
        p.isInvalid = false;
    }

    delete symtab;
    delete buffer;
}

bool plugin_manager::find_symbol(bfd* cur, asymbol** symtab, int nsym, std::string symbol, unsigned long& retval)
{
    for(int i = 0; i < nsym; i++)
    {
        if(symtab[i]->name == symbol)
        {
            //it is what we want
            symbol_info info;
            bfd_symbol_info(symtab[i], &info);

            retval = symtab[i]->section->filepos + symtab[i]->value;
            return true;
        }
    }
    retval = 0;
    return false;
}

