#include "language_menu.h"

#include "../common.h"
#include "../local/local.h"
#include "../local/local_strings.h"
#include "notcurses/nckeys.h"

#include <notcurses/notcurses.h>

extern struct notcurses* nc;
extern struct ncplane* stdplane;

// === Internal Functions ===
void update_language_menu_local(void);

void init_language_menu() {
    // update local once, so the strings are initialized
    update_language_menu_local();
    // add update local function to the observer list
    add_local_observer(update_language_menu_local);
}


menu_result_t show_language_menu() {
    const char* menu_options[2];

    menu_options[0] = &local_strings[lame_language_english.idx].characters[0];
    menu_options[1] = &local_strings[lame_language_german.idx].characters[0];

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
                    set_language(language_en);
                } else if (selected_index == 1) {
                    // German selected
                    set_language(language_de);
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

void update_language_menu_local(void) {
    snprintf(local_strings[lame_language_english.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(lame_language_english.key));
    snprintf(local_strings[lame_language_german.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(lame_language_german.key));
}
