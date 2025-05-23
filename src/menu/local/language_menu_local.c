#include "language_menu_local.h"

#include "../../local/local_handler.h"

#include <stdlib.h>

char** language_menu_strings = NULL;

void update_language_menu_local(void) {
    for (int i = 0; i < MAX_LANGUAGE_MENU_STRINGS; i++) {
        if (language_menu_strings[i] != NULL) {
            free(language_menu_strings[i]);
        }
    }

    language_menu_strings[LANGUAGE_ENGLISH] = get_local_string("ENGLISH");
    language_menu_strings[LANGUAGE_GERMAN] = get_local_string("GERMAN");
}
