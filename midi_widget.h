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

//this class defines a midi interface for use within the other widgets

#include "context.h"
#include "audio_widget.h"
#include "port.h"

#include <map>
#include <queue>

#include <gtkmm/comboboxtext.h>
#include <gtkmm/box.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/checkbutton.h>

#include <portmidi.h>

class midi_widget : public audio_widget
{

    public:

        midi_widget(context* context);
        ~midi_widget();

        void process();
        void process_ui();
        void post_creation_callback();

        void push_midi_event(PmEvent message);
        bool event_filter(PmDeviceID device_id, midi_types type, int data1);

    protected:

        std::map<int, PmDeviceID> midi_device_list;

        Gtk::Box v_box;
        Gtk::Box h_box;

        Gtk::ComboBoxText midi_device_combo;
        Gtk::ComboBoxText type_combo;
        Gtk::SpinButton controller_number_button;
        Gtk::CheckButton normalize_output_button;
        void midi_device_combo_changed();
        void type_combo_value_changed();
        void controller_number_button_value_changed();
        void normalize_output_button_changed();

        PmDeviceID midi_current_device = -1;
        midi_types midi_current_type;
        int controller_current;
        bool normalize;

        port* output_port;

        //midi_event_queue
        PortMidiStream *midi_stream = nullptr;
        std::queue<PmEvent> midi_event_queue;

        int lastmsg = 0;
};