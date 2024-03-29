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

#include "context.h"
#include "mainwindow.h"

void context::log(std::string msg)
{
    program_context->log(msg);
}

void context::put_widget(audio_widget* widget)
{
    program_context->playfield_add_widget(widget);
}

void context::remove_widget(audio_widget* widget)
{
    program_context->playfield_remove_widget(widget);
}

void context::set_base_class(MainWindow* base_class)
{
    program_context = base_class;
}

void context::show_widget_properties(audio_widget* widget)
{
    program_context->build_widget_properties(widget);
}