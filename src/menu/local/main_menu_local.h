#ifndef MAIN_MENU_LOCAL_H
#define MAIN_MENU_LOCAL_H

enum main_menu_index {
    NEW_GAME_STR,
    CONTINUE_STR,
    SAVE_GAME_STR,
    LOAD_GAME_STR,
    CHANGE_LANGUAGE_STR,
    EXIT_STR,
    QUESTION_CONTINUE,
    QUESTION_EXIT,

    MAX_MAIN_MENU_STRINGS
};

extern char** main_menu_strings;

void update_main_menu_local(void);

#endif //MAIN_MENU_LOCAL_H
