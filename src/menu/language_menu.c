#include "language_menu.h"

#include "../common.h"
#include "../local/local_handler.h"
#include "notcurses/nckeys.h"
#include "local/language_menu_local.h"

#include <notcurses/notcurses.h>

extern struct notcurses* nc;
extern struct ncplane* stdplane;

int init_language_menu() {
    language_menu_strings = (char**) malloc(sizeof(char*) * MAX_LANGUAGE_MENU_STRINGS);
    RETURN_WHEN_NULL(language_menu_strings, 1, "Language Menu", "Failed to allocate memory for language menu strings.");

    for (int i = 0; i < MAX_LANGUAGE_MENU_STRINGS; i++) {
        language_menu_strings[i] = NULL;
    }

    // update local once, so the strings are initialized
    update_language_menu_local();
    // add update local function to the observer list
    observe_local(update_language_menu_local);
    return 0;
}


menu_result_t show_language_menu() {
    const char* menu_options[2];

    menu_options[0] = language_menu_strings[LANGUAGE_ENGLISH];
    menu_options[1] = language_menu_strings[LANGUAGE_GERMAN];

    menu_result_t res = MENU_CHANGE_LANGUAGE;

    int selected_index = 0;
    bool selection_active = true;
    while (selection_active) {
        const int menu_count = 2;
        draw_menu(menu_options, menu_count, selected_index);

        ncinput event;
        memset(&event, 0, sizeof(event));
        notcurses_get_blocking(nc, &event);

        if (!(event.evtype == NCTYPE_UNKNOWN || event.evtype == NCTYPE_PRESS)) { continue; }

        switch (event.id) {
            case NCKEY_UP:
                selected_index = (selected_index - 1 + menu_count) % menu_count;
                break;
            case NCKEY_DOWN:
                selected_index = (selected_index + 1) % menu_count;
                break;
            case NCKEY_ENTER: {
                if (selected_index == 0) {
                    // English selected
                    set_language(LANGE_EN);
                } else if (selected_index == 1) {
                    // German selected
                    set_language(LANGE_DE);
                }
                selection_active = false;
                break;
            }
            case 'c':
                if (!(event.modifiers & NCKEY_MOD_CTRL)) {
                    break;
                }
                res = MENU_EXIT;
                selection_active = false;
                break;
            case NCKEY_ESC:
                res = MENU_CONTINUE;
                selection_active = false;
                break;
            default:
                // do nothing for other keys
                break;
        }
    }

    return res;
}

void shutdown_language_menu() {
    if (language_menu_strings != NULL) {
        for (int i = 0; i < MAX_LANGUAGE_MENU_STRINGS; i++) {
            if (language_menu_strings[i] != NULL) {
                free(language_menu_strings[i]);
            }
        }
        free(language_menu_strings);
    }
}
