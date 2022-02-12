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

//class that exposes the logging capabilities of the right panel

#pragma once

#include <sigc++/slot.h>

#include <string>
#include <tuple>
#include <vector>

#include <portmidi.h>


//foward declaration of the audio_widget class
//this might work, however this is a bad bad thing
//we need to rethink the code base to avoid this kind of stuff
#include "audio_widget_fwd.h"
#include "midi_widget_fwd.h"

enum midi_types
{
    CONTROL_CHANGE = 0b1011,
};

struct context
{
    sigc::slot<void(std::string)> log;
    sigc::slot<void(audio_widget*)> put_widget;
    sigc::slot<void(audio_widget*)> remove_widget;
    sigc::slot<std::vector<std::pair<PmDeviceID, PmDeviceInfo*>>()> get_midi_active_devices;
    sigc::slot<void(midi_widget*)> register_midi_consumer;
};