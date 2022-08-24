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

#include "widget_manager.h"

audio_widget* widget_manager::create_widget(std::string name)
{
    for(auto it = widget_database.begin(); it < widget_database.end(); it++)
    {
        if(it->name == name)
        {
            //load
            return it->creator_function();
        }
    }
    return nullptr;
}

void widget_manager::register_widget(std::string name, std::string long_name, std::string description, audio_widget* (*creator_function)())
{
    //check if it exists first
    for(auto it = widget_database.begin(); it < widget_database.end(); it++)
    {
        if(it->name == name)
            return;
    }
    widget_database.push_back({name, long_name, description, creator_function});
}

std::vector<widget_manager::widget_entry>& widget_manager::get_widget_database()
{
    return widget_database;
}

std::string widget_manager::get_widget_long_name(std::string name)
{
    for(auto it = widget_database.begin(); it < widget_database.end(); it++)
    {
        if(it->name == name)
        {
            //load
            return it->long_name;
        }
    }
    return "";
}