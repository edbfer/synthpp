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

#include "audio_widget.h"

struct logger
{
    sigc::slot<void(std::string)> log;
    sigc::slot<void(audio_widget*)> put_widget;
    sigc::slot<void(audio_widget*)> remove_widget;
};