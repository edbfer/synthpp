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

#include "debug_widget.h"

#include <string>
#include <gtkmm/cssprovider.h>

debug_widget::debug_widget()
{
    set_label("Debug Widget!");
    set_size_request(500, 500);

    //do 5 outputs
    //load the style provideer
    for(int i = 0; i < 5; i++)
    {
        port* p = new port("o" + std::to_string(i), port::port_type::OUTPUT);
        add_port(p);
        //estes vão ser output
    }

}