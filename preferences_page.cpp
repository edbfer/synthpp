//
// Created by eferreira on 21.02.25.
//

#include "preferences_page.h"

preferences_page::preferences_page(const audio_widget& widget)
    : widget(widget)
{
    p_page = std::unique_ptr<AdwPreferencesPage>(reinterpret_cast<AdwPreferencesPage*>(adw_preferences_page_new()));
}

void preferences_page::populate()
{
    //name group
    AdwPreferencesGroup* name_group = (AdwPreferencesGroup*) adw_preferences_group_new();
    adw_preferences_page_set_name(p_page.get(), "Widget Preferences");
    adw_preferences_page_add(p_page.get(), name_group);

    //create the rows for the name and class name
    AdwEntryRow* name_row = reinterpret_cast<AdwEntryRow*>(adw_entry_row_new());
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(name_row), ("Class name: " + widget.name).c_str());
    gtk_editable_set_text(GTK_EDITABLE(name_row), widget.long_name.c_str());
    adw_preferences_group_add(name_group, GTK_WIDGET(name_row));

    //now parameters
    AdwPreferencesGroup* parameters_group = (AdwPreferencesGroup*) adw_preferences_group_new();
    adw_preferences_group_set_title(parameters_group, "Parameters");
    adw_preferences_group_set_description(parameters_group, "Add, Remove and modify parameters");
    gtk_widget_set_hexpand(GTK_WIDGET(parameters_group), false);
    adw_preferences_page_add(p_page.get(), parameters_group);
}

