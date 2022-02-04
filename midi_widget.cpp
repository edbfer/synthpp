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

#include "midi_widget.h"
#include <tuple>
#include <iostream>

midi_widget::midi_widget()
{
    //enumerate devices
    int cnt = Pm_CountDevices();
    int iter = 0;
    for(int i = 0; i < cnt; i++)
    {
        const PmDeviceInfo* info = Pm_GetDeviceInfo(i);
        std::cout << "Detected: " << info->name << " " << ((info->input) ? "I" : "i") << "/" << ((info->output) ? "O" : "o") << std::endl;
        if(info->input) //is input
            midi_device_list[iter++] = i;
    }

    //now we have a list of detected midi devices
    //create the ui

    set_label("MIDI input");
    set_css_style("widget.css", "widget");

    //create the textcombobox
    midi_device_combo.set_size_request(150, -1);
    for(std::pair<int, PmDeviceID> device : midi_device_list)
    {
        const PmDeviceInfo* info = Pm_GetDeviceInfo(device.second);
        midi_device_combo.insert(device.first, info->name);
    }
    put(midi_device_combo, 10, 30);
    set_size_request(300, 75);
    //connect signal changed
    midi_device_combo.signal_changed().connect(sigc::mem_fun(*this, &midi_device_combo_changed), true);

    //create port
    output_port = new port("midi:0", port::port_type::OUTPUT);
    add_port(output_port);
}

midi_widget::~midi_widget()
{
    delete output_port;
}

void midi_widget::midi_device_combo_changed()
{
    int selected = midi_device_combo.get_active_row_number();
    //get the device
    PmDeviceID dev_id = midi_device_list.at(selected);
    midi_current_device = dev_id;

    //openstream
    if(midi_stream != nullptr)
        Pm_Close(midi_stream);

    Pm_OpenInput(&midi_stream, dev_id, NULL, 32, NULL, NULL);
    
}

void midi_widget::process()
{
    PmEvent incoming_events[32]; //we defined max 32
    //are there messages waiting?
    if(Pm_Poll(midi_stream))
    {
        //how many
        int count = Pm_Read(midi_stream, incoming_events, 32);
        for(int i = 0; i < count; i++)
        {
            //add event to queue(if channel 0)
            //get status byte
            char status = Pm_MessageStatus(incoming_events[i].message);
            int channel = status & 0xF;

            if(channel == 0)
            {
                //check if it is a control change
                int type = (status & 0xF0) >> 4;
                if(type == 0b1011)
                {
                    midi_event_queue.push(incoming_events[i]);
                }
            }

        }
    }

    //lets deal with the queue, just one event
    if(midi_event_queue.size() != 0)
    {
        PmEvent pe = midi_event_queue.front();
        midi_event_queue.pop();

        int msg2 = Pm_MessageData2(pe.message);

        output_port->push_sample(msg2);
        lastmsg = msg2;
    } else
    {
        output_port->push_sample(lastmsg);
    }
}

void midi_widget::process_ui()
{}

void midi_widget::post_creation_callback()
{
    set_ready(true);
}