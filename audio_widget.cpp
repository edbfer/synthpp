// Copyright (C) 2021 eduardof
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

#include <cassert>
#include <cmath>

#include <gtk/gtk.h>

#include "mainwindow.h"

#include "audio_widget.h"

audio_widget::audio_widget(int x_pos, int y_pos)
{
    this->x_pos = x_pos;
    this->y_pos = y_pos;

    base_class = (GtkFixed*) gtk_fixed_new();
    label = (GtkLabel*) gtk_label_new("");

    set_label("Empty widget");
    gtk_widget_set_size_request(GTK_WIDGET(base_class), 200, 200);

    gtk_fixed_put(base_class, GTK_WIDGET(label), 0, 0);

    //event
    gesture_drag = (GtkGestureDrag*) gtk_gesture_drag_new();
    gtk_widget_add_controller(GTK_WIDGET(base_class), GTK_EVENT_CONTROLLER(gesture_drag));

    //pressed event
    g_signal_connect(gesture_drag, "drag-begin", G_CALLBACK(mouse_grab_callback), this);
    g_signal_connect(gesture_drag, "drag-update", G_CALLBACK(mouse_grab_update_callback), this);

    gesture_click = (GtkGestureClick*) gtk_gesture_click_new();
    gtk_widget_add_controller(GTK_WIDGET(base_class), GTK_EVENT_CONTROLLER(gesture_click));

    g_signal_connect(gesture_click, "pressed", G_CALLBACK(mouse_click_callback), this);

    ////add handler to mapped fixed_canvas
    g_signal_connect(base_class, "map", G_CALLBACK(_base_class_on_creation_callback), this);
    g_signal_connect(base_class, "realize", G_CALLBACK(_base_class_post_creation_callback), this);

    //create css provider
    css_provider = gtk_css_provider_new();
    GtkStyleContext* sctx = gtk_widget_get_style_context(GTK_WIDGET(base_class));
    gtk_style_context_add_provider(sctx, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_THEME);

    isReady = false;
}

void audio_widget::mouse_grab_callback(GtkGestureDrag* gdrag, double x, double y, audio_widget* widget)
{
    //std::cout << "Pressed at (" << x << ", " << y << ")" << std::endl;
    //we only move on the top!
    if (y > 50)
        widget->ignore_mouse_drag = 1;
    else
        widget->ignore_mouse_drag = 0;
}

void audio_widget::mouse_grab_update_callback(GtkGestureDrag* gdrag, double offset_x, double offset_y, audio_widget* widget)
{
    double x = 500., y = 500.;

    //set_label("Moved!");
    
    GtkFixed* parent = (GtkFixed*) gtk_widget_get_parent(GTK_WIDGET(widget->base_class));
    gtk_fixed_get_child_position(parent, GTK_WIDGET(widget->base_class), &x, &y);

    //parent limits
    double parent_max_x, parent_max_y;
    GtkAllocation palloc;
    gtk_widget_get_allocation(GTK_WIDGET(parent), &palloc);
    parent_max_x = palloc.width;
    parent_max_y = palloc.height;

    x = std::max(std::min(x + offset_x, parent_max_x), 0.);
    y = std::max(std::min(y + offset_y, parent_max_y), 0.);

    widget->x_pos = x;
    widget->y_pos = y;

    if(!(widget->ignore_mouse_drag))
    {
        gtk_fixed_move(parent, GTK_WIDGET(widget->base_class), x, y);

        //update the position of the ports in the drawing ara
        for(port* p : widget->port_vector)
        {
            //get the position of the port in the widget
            int port_x, port_y;
            p->get_position_inwidget(port_x, port_y);

            //if we are input, we are actually already on the border, if not we need the width of the label
            int wid = (p->get_direction() == port::port_type::OUTPUT) ? p->get_allocation().width : 0;

            p->set_position_indarea(port_x + x + wid, port_y + y);
        }    

        //update the drawing area
        //((MainWindow*) ((Gtk::Grid*) parent->get_parent())->get_parent())->playfield_trigger_redraw();
    }
    //std::cout << "curpos: " << x << ", " << y << std::endl;
    
}

audio_widget::~audio_widget()
{
    for(port* p: this->port_vector)
        delete p;
}

void audio_widget::add_port(port* p)
{
    port_vector.push_back(p);

    //get the position of the widget in the darea
    int wid_x = this->x_pos;
    int wid_y = this->y_pos;

    //lets wait for p to be realized:
    //p->signal_show().connect(sigc::bind(sigc::mem_fun(*this, &audio_widget::on_port_realize), p));

    //need the size
    PangoLayout* playout = gtk_widget_create_pango_layout(GTK_WIDGET(p->get_base_class()), p->get_label().c_str());

    //place on the drawing
    if(p->get_direction() == port::port_type::OUTPUT)
    {
        //get number of ports
        n_out_ports++;

        int position_x, position_y;
        gtk_widget_get_size_request(GTK_WIDGET(base_class), &position_x, &position_y);

        int wid, hei;
        pango_layout_get_size(playout, &wid, &hei);

        wid /= PANGO_SCALE;
        hei /= PANGO_SCALE;

        position_x -= wid;
        position_y = 10 + n_out_ports*20;

        gtk_fixed_put(base_class, GTK_WIDGET(p->get_base_class()), position_x, position_y);

        p->set_position_inwidget(position_x, position_y);

        //in drawing area
        p->set_position_indarea(position_x + wid_x + wid, position_y + wid_y);
    } 
    else if(p->get_direction() == port::port_type::INPUT)
    {
        //get number of ports
        n_in_ports++;

        int position_x, position_y;
        gtk_widget_get_size_request(GTK_WIDGET(base_class), &position_x, &position_y);

        position_x = 0;
        position_y = 10 + n_in_ports*20;

        gtk_fixed_put(base_class, GTK_WIDGET(p->get_base_class()), position_x, position_y);
        p->set_position_inwidget(position_x, position_y);

        //in drawing area
        p->set_position_indarea(position_x + wid_x, position_y + wid_y);
    }
}

std::vector<port*>* audio_widget::get_port_list()
{
    return &port_vector;
}

void audio_widget::get_underlaying_fixed_position(int& x, int& y)
{
    GtkAllocation alloc;
    gtk_widget_get_allocation(GTK_WIDGET(base_class), &alloc);

    x = alloc.x;
    y = alloc.y;
}

void audio_widget::on_port_realize(port* p)
{
    //Gtk::Fixed* fixed_parent = (Gtk::Fixed*) p->get_parent();
    //GtkFixed

    //int fixed_x = fixed_canvas.get_allocation().get_x();
    //int fixed_y = fixed_canvas.get_allocation().get_y();
}

void audio_widget::set_label(std::string label)
{
    gtk_label_set_text(this->label, label.c_str());
}

void audio_widget::set_css_style(std::string filename, std::string css_class)
{
    gtk_css_provider_load_from_path(css_provider, ("styles/" + filename).c_str());
    gtk_widget_add_css_class(GTK_WIDGET(base_class), css_class.c_str());
}

void audio_widget::set_size_request(int w, int h)
{
    gtk_widget_set_size_request(GTK_WIDGET(base_class), w, h);
}

void audio_widget::set_ready(bool ready)
{
    isReady = ready;
}

bool audio_widget::is_ready()
{
    return isReady;
}

void audio_widget::_base_class_post_creation_callback(GtkFixed* fxd, audio_widget* widget)
{
    widget->post_creation_callback();
}

void audio_widget::_base_class_on_creation_callback(GtkFixed* fxd, audio_widget* widget)
{
    widget->on_creation_callback();
}

void audio_widget::add_control(control_type type, std::string name, std::string parameter)
{
    ui_control new_control;

    //fill the struct
    new_control.name = name;
    new_control.text = name;
    new_control.type = type;
    new_control.parameter_index = find_parameter_id(parameter);
    new_control.max_value = 1.0;
    new_control.min_value = 0.0;
    new_control.step = 0.1;

    //create the control
    switch(type)
    {
        case control_type::label:
        {
            new_control.widget = gtk_label_new(name.c_str());
            break;
        }

        case control_type::scale:
        {
            new_control.widget = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, new_control.min_value, new_control.max_value, new_control.step);

            //assign the callback
            g_signal_connect(new_control.widget, "value-changed", G_CALLBACK(_base_class_scale_control_callback), this);
            break;
        }

        case control_type::file_selector:
        {
            new_control.widget = gtk_file_chooser_dialog_new("Select a file", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_APPLY, NULL);
            gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(new_control.widget), false);

            //assign the callback
            g_signal_connect(new_control.widget, "response", G_CALLBACK(_base_class_file_selector_control_callback), this);
        }
    }

    //push the control
    int size = control_list.size();
    control_list.push_back(new_control);

    //put the widget
    gtk_widget_set_size_request(GTK_WIDGET(new_control.widget), 150, 50);
    gtk_fixed_put(base_class, GTK_WIDGET(new_control.widget), 15, 30 + size*55);
}

void audio_widget::set_control_text(std::string name, std::string text)
{
    //this should be refactored into mini structs
    for(ui_control& control : control_list)
    {
        if(control.name == name)
        {
            control.text = text;
            
            switch(control.type)
            {
                case control_type::label:
                {
                    gtk_label_set_label(GTK_LABEL(control.widget), text.c_str());
                    break;
                }
            }
        }
    }
}

bool audio_widget::find_control_id(std::string name, int& id)
{
    for(auto i = control_list.begin(); i < control_list.end(); i++)
    {
        if(i->name.compare(name) == 0)
        {
            id = i - control_list.begin();
            return true;
        }
    }
    return false;
}

int audio_widget::find_parameter_id(std::string name)
{
    for(auto i = parameter_list.begin(); i < parameter_list.end(); i++)
    {
        if(i->name.compare(name) == 0)
        {
            return  i - parameter_list.begin();
        }
    }
    return -1;
}

void audio_widget::add_parameter(std::string name, parameter_types type, std::variant<float, std::string> starting_value)
{
    parameter_list.push_back({name, type, starting_value});
}

bool audio_widget::find_control_by_widget_pointer(GtkWidget* widget, ui_control& control)
{
    for(ui_control ctrl : control_list)
    {
        if(widget == ctrl.widget)
        {
            control = ctrl;
            return true;
        }
    }
    return false;
}

void audio_widget::_base_class_scale_control_callback(GtkRange* scale, audio_widget* widget)
{
    //find the control
    ui_control ctrl;
    if(widget->find_control_by_widget_pointer(GTK_WIDGET(scale), ctrl))
    {
        int idx = ctrl.parameter_index;
        //this should be numeric already, the previous code should not allow a type mismatch
        widget->parameter_list.at(idx).val = (float) gtk_range_get_value(GTK_RANGE(scale));
    }
}

float audio_widget::get_numerical_parameter_value(std::string name)
{
    //check type 
    int id = find_parameter_id(name);
    return (parameter_list.at(id).type == parameter_types::NUMERIC) ? std::get<float>(parameter_list.at(id).val) : NAN;
}

void audio_widget::_base_class_process()
{
    //lets first do the linking and then only pass onto the processing 
    for(link l : link_list)
    {
        port* p = port_vector.at(l.port_id);
        ui_control& ui = control_list.at(l.control_id);

        //again this will be a numerical value already
        parameter_list.at(ui.parameter_index).val = p->pop_sample();
        control_match_to_param(ui);
    }
}

bool audio_widget::find_port(std::string name, port ** out)
{
    for(auto i = port_vector.begin(); i < port_vector.end(); i++)
    {
        port* p = *i;
        if(p->get_label().compare(name) == 0)
        {
            *out = p;
            return true;
        }
    }

    return false;
}

bool audio_widget::find_port_id(std::string name, int& id)
{
    for(auto i = port_vector.begin(); i < port_vector.end(); i++)
    {
        if((*i)->get_label().compare(name) == 0)
        {
            id = i - port_vector.begin();
            return true;
        }
    }
    return false;
}

void audio_widget::control_match_to_param(ui_control& ui)
{
    int id = ui.parameter_index;
    switch(ui.type)
    {
        case control_type::scale:
        {
            gtk_range_set_value(GTK_RANGE(ui.widget), std::get<float>(parameter_list.at(id).val));
            break;
        }

        case control_type::file_selector:
        {
            break;
        }
    }
}

void audio_widget::link_port_to_control(std::string linkname, std::string port, std::string control)
{
    int port_id;
    int control_id;
    if(!find_port_id(port, port_id)) return;
    if(!find_control_id(control, control_id)) return;

    link_list.push_back({linkname, port_id, control_id});
}

void audio_widget::_base_class_file_selector_control_callback(GtkFileChooserDialog* dialog, int response, audio_widget* widget)
{
    ui_control ctrl;
    if(widget->find_control_by_widget_pointer(GTK_WIDGET(dialog), ctrl))
    {
        if(response == GTK_RESPONSE_APPLY)
        {
            GFile* selection = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
            char* filepath = g_file_get_path(selection);
            ctrl.val = std::string(filepath);

            int idx = ctrl.parameter_index;
            widget->parameter_list.at(idx).val = ctrl.val;

            //cleanup
            g_free(filepath);
            g_object_unref(selection);
        }
    }
}

void audio_widget::mouse_click_callback(GtkGestureClick* gclick, int n_presses, double x, double y, audio_widget* widget)
{
    int nbutton = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gclick));
    if(nbutton == 1)
    {
        widget->program_context->show_widget_properties(widget);
    }
}
