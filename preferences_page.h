//
// Created by eferreira on 21.02.25.
//

#ifndef PREFERENCES_PAGE_H
#define PREFERENCES_PAGE_H

#include <adwaita.h>
#include <memory>

#include "audio_widget.h"

class preferences_page
{
    public:
        explicit preferences_page(const audio_widget& widget);
        void populate();

    private:

        std::unique_ptr<AdwPreferencesPage> p_page;
        const audio_widget& widget;
};

#endif //PREFERENCES_PAGE_H
