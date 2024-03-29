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
#include "audio_widget.h"
#include <vector>

class widget_manager
{
    public:

        struct widget_entry
        {
            std::string name;
            std::string long_name;
            std::string description;
            audio_widget* (*creator_function) ();
        };

        audio_widget* create_widget(std::string name);

        void register_widget(std::string name, std::string long_name, std::string description, audio_widget* (*creator_function) ());

        std::vector<widget_entry>& get_widget_database();
        
        std::string get_widget_long_name(std::string name);

    private:
        std::vector<widget_entry> widget_database;
};