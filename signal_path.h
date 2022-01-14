// Copyright (C) 2021 Eduardo Ferreira
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

#include "port.h"
#include <string>
#include <random>

class signal_path
{

    public:
        signal_path(port* source = nullptr, port* dest = nullptr);

        //void propagate(int n_samples);
        void propagate_input();
        void propagate_output();

        port* get_source_port();
        void set_source_port(port* source);

        port* get_destination_port();
        void set_destination_port(port* dest);
        
        std::string get_path_name();

        bool is_ui_selected();
        void set_ui_selected(bool ui_selected);

        bool bounds_check(float mx, float my, float delta);

    protected:
        std::string path_name;

        float transport;

        bool ui_selected;

        port* source_port;
        port* destination_port;
};