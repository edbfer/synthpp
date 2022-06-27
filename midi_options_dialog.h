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

//dialog for selecting used midi devices

#pragma once

#include <gtk/gtk.h>

#include "audio_engine.h"

/*class midi_list_record : public Gtk::TreeModel::ColumnRecord
{
    public:

        Gtk::TreeModelColumn<int> device_id;
        Gtk::TreeModelColumn<Glib::ustring> device_name;
        Gtk::TreeModelColumn<bool> device_is_enabled;
    
        midi_list_record()
        {
            add(device_id);
            add(device_name);
            add(device_is_enabled);
        }
};*/


class midi_options_dialog
{
    public:

        midi_options_dialog(GtkWindow* parent, audio_engine* engine);
        void show();

    protected:

        GtkWindow* parent;
        GtkDialog* base_class;

        //explanation label
        GtkLabel* explanation_label;

        //container
        GtkBox* v_box;

        //container of midi devices
        GtkTreeView* midi_tree_view;
        GtkListStore* midi_list_store;

        GtkTreeViewColumn* name_column;
        GtkTreeViewColumn* sel_column;

        audio_engine* engine;

        static void cell_renderer_toggled(GtkCellRendererToggle* tgl, gchar* path, midi_options_dialog* dialog);
        static void dialog_response(GtkDialog* dlg, int response, midi_options_dialog* dialog);
        static void mapped_signal(GtkDialog* dlg, midi_options_dialog* dialog);
};