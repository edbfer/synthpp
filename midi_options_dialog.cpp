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
    set_size_request(500, 500);
    
    v_box.set_orientation(Gtk::Orientation::VERTICAL);
    v_box.set_hexpand(true);
    v_box.set_vexpand(true);
    v_box.set_valign(Gtk::Align::FILL);
    v_box.set_halign(Gtk::Align::FILL);
    set_child(v_box);

    explanation_label.set_text("synthpp uses MIDI channel 0, by default");
    explanation_label.set_hexpand(true);
    v_box.append(explanation_label);

    //dialog buttons
    add_button("Cancel", Gtk::ResponseType::CANCEL);
    add_button("Apply", Gtk::ResponseType::APPLY);
}