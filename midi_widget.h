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

#include "audio_widget.h"
#include "port.h"

#include <map>
#include <queue>

#include <gtkmm/comboboxtext.h>

#include <portmidi.h>

class midi_widget : public audio_widget
{

    public:

        midi_widget();
        ~midi_widget();

        void process();
        void process_ui();
        void post_creation_callback();

    protected:

        std::map<int, PmDeviceID> midi_device_list;

        Gtk::ComboBoxText midi_device_combo;
        void midi_device_combo_changed();

        PmDeviceID midi_current_device = -1;

        port* output_port;

        //midi_event_queue
        PortMidiStream *midi_stream = nullptr;
        std::queue<PmEvent> midi_event_queue;

        int lastmsg = 0;

};