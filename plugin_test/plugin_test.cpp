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

#include "../audio_widget.h"

#include <string>

#define PLUGIN_METADATA __attribute__ ((section("synthpp-metadata")))

class test_plugin : public audio_widget
{
    public:

        test_plugin() : audio_widget() {}

        void on_creation_callback()
        {
            set_label("teste widget");
            set_css_style("widget.css", "widget");

            add_parameter("param", 1.0f);
            add_control(control_type::scale, "Ola", "param");
        }

        void post_creation_callback() 
        {
            set_ready(true);
        }

        void process() {}
        void process_ui() {
            set_label(std::to_string(get_parameter_value("param")));
        }
};

extern "C" {
    std::string get_magic()
    {
        return "SynthPP-Plugin-v1";
    }

    const char* PLUGIN_METADATA magic_string = "SynthPP-Plugin-v1";
    const char* PLUGIN_METADATA plugin_name = "org.edbfer.synthpp.test-plugin";
    const char* PLUGIN_METADATA plugin_long_name = "Test plugin";
    const char* PLUGIN_METADATA plugin_description = "This is a test plugin!";

    std::string get_plugin_name()
    {
        return "org.edbfer.synthpp.test-plugin";
    }

    audio_widget* create_widget_instance()
    {
        test_plugin* tp = new test_plugin();
        return tp;
    }
}

