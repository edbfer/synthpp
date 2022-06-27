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

#pragma once

#include <string>

#include <gtk/gtklabel.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtkscale.h>

enum control_type{
    label,
    button,
    scale
};

struct ui_control{
    std::string name;
    control_type type;
    int parameter_index;

    GtkWidget* widget;

    std::string text;
    float max_value;
    float min_value;
    float step;
};