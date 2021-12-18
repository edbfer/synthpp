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

#include "utils.h"

#include <random>
#include <chrono>

char charset[16] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'a', 'b', 'c', 'd', 'e', 'f'};

std::string utils::gen_8char_id()
{
    std::chrono::time_point seed = std::chrono::high_resolution_clock::now();

    std::default_random_engine dr_engine(seed.time_since_epoch().count());

    std::uniform_int_distribution<int> uint_distribution(0, 15);
    
    std::string output;
    for(int i = 0; i < 8; i++)
    {
        int next_int = uint_distribution(dr_engine);
        char to_add = charset[next_int];
        output += to_add;
    }

    return output;
}