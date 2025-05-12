#ifndef LANGUAGE_MENU_LOCAL_H
#define LANGUAGE_MENU_LOCAL_H

enum language_menu_index {
    LANGUAGE_ENGLISH,
    LANGUAGE_GERMAN,

    MAX_LANGUAGE_MENU_STRINGS
};

extern char** language_menu_strings;

void update_language_menu_local(void);

#endif //LANGUAGE_MENU_LOCAL_H
