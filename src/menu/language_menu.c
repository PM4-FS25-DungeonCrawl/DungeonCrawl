#include "language_menu.h"

#include "../common.h"
#include "../io/input/input_handler.h"
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

        input_event_t input_event;
        if (!get_input_blocking(&input_event)) {
            continue;
        }

        // Use our logical input types
        switch (input_event.type) {
            case INPUT_UP:
                selected_index = (selected_index - 1 + menu_count) % menu_count;
                break;
            case INPUT_DOWN:
                selected_index = (selected_index + 1) % menu_count;
                break;
            case INPUT_CONFIRM: {
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
            case INPUT_QUIT:
                res = MENU_EXIT;
                selection_active = false;
                break;
            case INPUT_CANCEL:
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
