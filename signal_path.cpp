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

#include "signal_path.h"
#include "utils.h"

signal_path::signal_path(port* source, port* dest)
{
    this->source_port = source;
    this->destination_port = dest;

    std::string name = utils::gen_8char_id();
    this->path_name = name;
}

port* signal_path::get_source_port()
{
    return source_port;
}

port* signal_path::get_destination_port()
{
    return destination_port;
}

std::string signal_path::get_path_name()
{
    return path_name;
}

void signal_path::set_source_port(port* source)
{
    this->source_port = source;
}

void signal_path::set_destination_port(port* dest)
{
    this->destination_port = dest;
}