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

#include "audio_widget.h"

class sink_widget : public audio_widget
{
    public:
        sink_widget(int nports);
        ~sink_widget();

        void on_creation_callback();

        void post_creation_callback();
        void process();
        void process_ui();

        float* get_samples();

    protected:

        int nports;

        //this has the last played samples -> n_ports elements
        float* samples;
};