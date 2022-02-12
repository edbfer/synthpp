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

#include "midi_options_dialog.h"

midi_options_dialog::midi_options_dialog(audio_engine* engine) : Gtk::Dialog("MIDI Options", true, true)
{
    this->engine = engine;

    //construct the dialog
    set_title("MIDI Options");
    set_size_request(500, 150);
    
    v_box.set_orientation(Gtk::Orientation::VERTICAL);
    v_box.set_hexpand(true);
    v_box.set_vexpand(true);
    v_box.set_valign(Gtk::Align::FILL);
    v_box.set_halign(Gtk::Align::FILL);
    set_child(v_box);

    explanation_label.set_text("synthpp uses MIDI channel 0, by default");
    explanation_label.set_hexpand(true);
    explanation_label.set_margin(5.0f);
    v_box.append(explanation_label);

    //dialog buttons
    add_button("Cancel", Gtk::ResponseType::CANCEL);
    add_button("Apply", Gtk::ResponseType::APPLY);

    //creates the store
    midi_list_store = Gtk::ListStore::create(midi_cols);
    midi_tree_view.set_hexpand(true);
    midi_tree_view.set_vexpand(true);
    midi_tree_view.set_halign(Gtk::Align::FILL);
    midi_tree_view.set_valign(Gtk::Align::FILL);
    midi_tree_view.set_margin(5.0f);
    midi_tree_view.set_headers_visible(true);
    midi_tree_view.set_model(midi_list_store);
    v_box.append(midi_tree_view);

    //populate list
    auto device_vector = engine->get_midi_device_vector();

    for(auto device : device_vector)
    {
        Gtk::TreeModel::iterator iter = midi_list_store->append();
        iter->set_value(0, device.first);
        iter->set_value(1, std::string(device.second->name));
        iter->set_value(2, engine->is_midi_device_enabled(device.first));
    }

    midi_tree_view.append_column("Device name", midi_cols.device_name);
    midi_tree_view.append_column_editable("Enabled", midi_cols.device_is_enabled);

    //resize cols
    midi_tree_view.get_column(1)->set_expand(false);
    midi_tree_view.get_column(0)->set_expand(true);

    signal_response().connect(sigc::mem_fun(*this, &dialog_response));
}

void midi_options_dialog::dialog_response(int response)
{
    if(response == Gtk::ResponseType::APPLY)
    {
        for(auto row : midi_list_store->children())
        {
            PmDeviceID dev_id = row.get_value(midi_cols.device_id);
            bool is_enabled = row.get_value(midi_cols.device_is_enabled);
            engine->midi_device_set_enabled(dev_id, is_enabled);
        }

        close();
    }
    else if(response == Gtk::ResponseType::CANCEL)
    {
        close();
    }
}