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

#include <gtkmm/dialog.h>
#include <gtkmm/label.h>
#include <gtkmm/box.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>

#include "audio_engine.h"

class midi_list_record : public Gtk::TreeModel::ColumnRecord
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
};


class midi_options_dialog : public Gtk::Dialog
{
    public:

        midi_options_dialog(audio_engine* engine);

    protected:

        //explanation label
        Gtk::Label explanation_label;

        //container
        Gtk::Box v_box;

        //container of midi devices
        midi_list_record midi_cols;
        Gtk::TreeView midi_tree_view;
        Glib::RefPtr<Gtk::ListStore> midi_list_store;

        audio_engine* engine;

        void dialog_response(int response);
};