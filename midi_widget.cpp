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

midi_widget::midi_widget(context* program_context) : audio_widget(program_context)
{
    //now we have a list of detected midi devices
    //create the ui
    set_label("MIDI input");
    set_css_style("widget.css", "widget");
    set_size_request(300, 75);

    //boxes
    v_box.set_orientation(Gtk::Orientation::VERTICAL);
    v_box.set_expand(true);
    v_box.set_valign(Gtk::Align::FILL);
    v_box.set_halign(Gtk::Align::FILL);
    v_box.set_margin(3);
    put(v_box, 10, 30);

    //create the textcombobox
    midi_device_combo.set_size_request(200, -1);
    int i = 0;
    for(auto device : program_context->get_midi_active_devices())
    {
        midi_device_combo.insert(i++, device.second->name);
    }
    v_box.prepend(midi_device_combo);

    //prepend hbox
    h_box.set_hexpand(true);
    h_box.set_halign(Gtk::Align::FILL);
    v_box.append(h_box);

    //create textcombobox for type
    type_combo.set_expand(true);
    type_combo.set_halign(Gtk::Align::FILL);
    type_combo.insert(0, "Control Change");
    h_box.prepend(type_combo);

    //add spin button
    Glib::RefPtr<Gtk::Adjustment> controller_spin_adj = controller_number_button.get_adjustment();
    controller_spin_adj->set_lower(0);
    controller_spin_adj->set_upper(127);
    controller_spin_adj->set_step_increment(1);
    h_box.append(controller_number_button);

    //checkbutton
    normalize_output_button.set_expand(true);
    normalize_output_button.set_halign(Gtk::Align::FILL);
    normalize_output_button.set_label("Normalize output");
    normalize_output_button.set_active(true);
    normalize = true;
    v_box.append(normalize_output_button);

    //connect signal changed
    midi_device_combo.signal_changed().connect(sigc::mem_fun(*this, &midi_device_combo_changed), true);
    type_combo.signal_changed().connect(sigc::mem_fun(*this, &type_combo_value_changed));
    controller_number_button.signal_changed().connect(sigc::mem_fun(*this, &controller_number_button_value_changed));
    normalize_output_button.signal_toggled().connect(sigc::mem_fun(*this, &normalize_output_button_changed));

    //create port
    output_port = new port("out", port::port_type::OUTPUT);
    add_port(output_port);

    //register midi consumer
    program_context->register_midi_consumer(this);
}

midi_widget::~midi_widget()
{
    delete output_port;
}

void midi_widget::midi_device_combo_changed()
{
    int selected_item = midi_device_combo.get_active_row_number();

    //get pmdeviceid
    midi_current_device = program_context->get_midi_active_devices().at(selected_item).first;
}

void midi_widget::type_combo_value_changed()
{
    int selected_item = type_combo.get_active_row_number();

    std::cout << "item: " << selected_item << std::endl;
    
    switch(selected_item)
    {
        case 0:
        {
            midi_current_type = midi_types::CONTROL_CHANGE;
            break;
        }
    }
}

void midi_widget::controller_number_button_value_changed()
{
    controller_current = controller_number_button.get_value_as_int();
}

void midi_widget::process()
{
    //get active queue
    if(!midi_event_queue.empty()) 
    {   
        PmEvent evt = midi_event_queue.front();
        lastmsg = Pm_MessageData2(evt.message);
        midi_event_queue.pop();
    }
    
    output_port->push_sample((normalize) ? lastmsg / 127.f : lastmsg);
}

void midi_widget::process_ui()
{}

void midi_widget::post_creation_callback()
{
    set_ready(true);
}

void midi_widget::push_midi_event(PmEvent event)
{
    midi_event_queue.push(event);
}

bool midi_widget::event_filter(PmDeviceID device_id, midi_types type, int data1)
{
    return ((device_id == midi_current_device) && (type == midi_current_type) && (data1 == controller_current));
}

void midi_widget::normalize_output_button_changed()
{
    normalize = normalize_output_button.get_active();
}