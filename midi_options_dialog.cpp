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

#include "midi_options_dialog.h"

#include <glib.h>
#include <gtk/gtkcellrenderertext.h>

midi_options_dialog::midi_options_dialog(GtkWindow* parent, audio_engine* engine)
{
    this->engine = engine;
    this->parent = parent;

    base_class = (GtkDialog*) gtk_dialog_new_with_buttons("MIDI Options", parent, GTK_DIALOG_MODAL, "Apply", GTK_RESPONSE_APPLY, "Cancel", GTK_RESPONSE_CANCEL, NULL);

    g_signal_connect(base_class, "map", G_CALLBACK(mapped_signal), this);
}

void midi_options_dialog::mapped_signal(GtkDialog* dlg, midi_options_dialog* dialog)
{
    //construct the dialog
    gtk_window_set_title(GTK_WINDOW(dialog->base_class), "MIDI Options");
    gtk_widget_set_size_request(GTK_WIDGET(dialog->base_class), 500, 150);
    
    //box
    dialog->v_box = GTK_BOX(gtk_dialog_get_content_area(dialog->base_class));
    /*gtk_widget_set_hexpand(GTK_WIDGET(v_box), true);
    gtk_widget_set_vexpand(GTK_WIDGET(v_box), true);
    gtk_widget_set_halign(GTK_WIDGET(v_box), GTK_ALIGN_FILL);
    gtk_widget_set_valign(GTK_WIDGET(v_box), GTK_ALIGN_FILL);
    gtk_window_set_child(GTK_WINDOW(base_class), GTK_WIDGET(v_box));*/

    //label
    dialog->explanation_label = (GtkLabel*) gtk_label_new("synthpp uses MIDI channel 0, by default");
    gtk_widget_set_hexpand(GTK_WIDGET(dialog->explanation_label), true);
    gtk_box_append(dialog->v_box, GTK_WIDGET(dialog->explanation_label));

    /*//dialog buttons
    gtk_dialog_add_button(base_class, "Cancel", GTK_RESPONSE_CANCEL);
    gtk_dialog_add_button(base_class, "Apply", GTK_RESPONSE_APPLY);*/

    //creates the store
    dialog->midi_list_store = (GtkListStore*) gtk_list_store_new(3, G_TYPE_INT, G_TYPE_STRING, G_TYPE_BOOLEAN);

    dialog->midi_tree_view = (GtkTreeView*) gtk_tree_view_new();
    gtk_tree_view_set_headers_visible(dialog->midi_tree_view, true);
    gtk_tree_view_set_model(dialog->midi_tree_view, GTK_TREE_MODEL(dialog->midi_list_store));

    gtk_widget_set_hexpand(GTK_WIDGET(dialog->midi_tree_view), true);
    gtk_widget_set_vexpand(GTK_WIDGET(dialog->midi_tree_view), true);
    gtk_widget_set_halign(GTK_WIDGET(dialog->midi_tree_view), GTK_ALIGN_FILL);
    gtk_widget_set_valign(GTK_WIDGET(dialog->midi_tree_view), GTK_ALIGN_FILL);
    gtk_box_append(dialog->v_box, GTK_WIDGET(dialog->midi_tree_view));

    //populate list
    auto device_vector = dialog->engine->get_midi_device_vector();

    for(auto device : device_vector)
    {
        GtkTreeIter iter;
        gtk_list_store_append(dialog->midi_list_store, &iter);

        gboolean val2 = dialog->engine->is_midi_device_enabled(device.first);
        GValue c1 = G_VALUE_INIT; g_value_init(&c1, G_TYPE_INT); g_value_set_int(&c1, device.first);
        GValue c2 = G_VALUE_INIT; g_value_init(&c2, G_TYPE_STRING); g_value_set_string(&c2, device.second->name);
        GValue c3 = G_VALUE_INIT; g_value_init(&c3, G_TYPE_BOOLEAN); g_value_set_boolean(&c3, val2);

        gtk_list_store_set_value(dialog->midi_list_store, &iter, 0,  &c1);
        gtk_list_store_set_value(dialog->midi_list_store, &iter, 1, &c2);
        gtk_list_store_set_value(dialog->midi_list_store, &iter, 2, &c3);
    }

    GtkCellRenderer* renderer;

    renderer = gtk_cell_renderer_text_new();
    dialog->name_column = (GtkTreeViewColumn*)  gtk_tree_view_column_new_with_attributes("Device name", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(dialog->midi_tree_view, dialog->name_column);

    GtkCellRendererToggle* tg_renderer = GTK_CELL_RENDERER_TOGGLE(gtk_cell_renderer_toggle_new());

    gtk_cell_renderer_toggle_set_activatable(tg_renderer, true);
    //need to connect the signal to change the value in the store
    g_signal_connect(tg_renderer, "toggled", G_CALLBACK(cell_renderer_toggled), dialog);

    dialog->sel_column = (GtkTreeViewColumn*) gtk_tree_view_column_new_with_attributes("Enabled", GTK_CELL_RENDERER(tg_renderer), "active", 2, NULL);

    gtk_tree_view_append_column(dialog->midi_tree_view, dialog->sel_column);

    //resize cols
    gtk_tree_view_column_set_expand(dialog->name_column, true);
    gtk_tree_view_column_set_expand(dialog->sel_column, false);

    g_signal_connect(dialog->base_class, "response", G_CALLBACK(dialog_response), dialog);

    gtk_window_set_modal(GTK_WINDOW(dialog->base_class), true);
}

void midi_options_dialog::dialog_response(GtkDialog* dlg, int response, midi_options_dialog* dialog)
{
    if(response == GTK_RESPONSE_APPLY)
    {
        GtkTreeIter iterator;
        bool valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(dialog->midi_list_store), &iterator);

        while(valid)
        {
            int dev_id;
            bool is_enabled;

            gtk_tree_model_get(GTK_TREE_MODEL(dialog->midi_list_store), &iterator, 0, &dev_id, 2, &is_enabled, -1);
            dialog->engine->midi_device_set_enabled(dev_id, is_enabled);
        }

        gtk_window_close(GTK_WINDOW(dialog->base_class));
        delete dialog;
    }
    else if(response == GTK_RESPONSE_CANCEL)
    {
        gtk_window_close(GTK_WINDOW(dialog->base_class));
        delete dialog;
    }
}

void midi_options_dialog::show()
{
    //gtk_dialog_run()
    gtk_widget_show(GTK_WIDGET(base_class));
}

void midi_options_dialog::cell_renderer_toggled(GtkCellRendererToggle* tgl, gchar* path, midi_options_dialog* dialog)
{
    GtkTreePath* tpath = gtk_tree_path_new_from_string(path);

    //set value
    GtkTreeIter iter;
    gtk_tree_model_get_iter(GTK_TREE_MODEL(dialog->midi_list_store), &iter, tpath);

    gboolean cur_val;
    gtk_tree_model_get(GTK_TREE_MODEL(dialog->midi_list_store), &iter, 2, &cur_val, -1);

    gtk_list_store_set(dialog->midi_list_store, &iter, 2, !cur_val, -1);
}