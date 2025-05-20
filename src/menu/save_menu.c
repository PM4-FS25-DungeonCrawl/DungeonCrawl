#include "save_menu.h"

#include "../common.h"
#include "../database/database.h"
#include "../database/game/gamestate_database.h"
#include "../io/input/input_handler.h"
#include "../local/local_handler.h"
#include "local/save_menu_local.h"
#include "notcurses/nckeys.h"
#include "src/menu/menu.h"

#include <notcurses/notcurses.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>


int selected_save_file_id = -1;
char last_save_name[50] = {0};
extern struct notcurses* nc;
extern struct ncplane* stdplane;

int init_save_menu() {
    save_menu_strings = malloc(sizeof(char*) * MAX_SAVE_MENU_STRINGS);
    RETURN_WHEN_NULL(save_menu_strings, 1, "Save Menu", "Failed to allocate memory for save menu strings.");

    for (int i = 0; i < MAX_SAVE_MENU_STRINGS; i++) {
        save_menu_strings[i] = NULL;
    }

    // update local once, so the strings are initialized
    update_save_menu_local();
    // add update local function to the observer list
    observe_local(update_save_menu_local);
    return 0;
}


int get_selected_save_file_id(void) {
    return selected_save_file_id;
}

const char* get_save_name(void) {
    if (last_save_name[0] == '\0') {
        return NULL;
    }
    return last_save_name;
}

menu_result_t show_save_game_menu(void) {
    char save_name[50] = {0};
    int name_length = 0;
    bool input_active = true;
    menu_result_t result = MENU_CONTINUE;

    // Get save name from user
    while (input_active && name_length < 49) {
        // clear screen
        ncplane_set_channels(stdplane, DEFAULT_COLORS);
        for (uint i = 0; i < ncplane_dim_x(stdplane); i++) {
            for (uint j = 0; j < ncplane_dim_y(stdplane); j++) {
                ncplane_printf_yx(stdplane, (int) j, (int) i, " ");
            }
        }
        ncplane_set_channels(stdplane, DEFAULT_COLORS);
        ncplane_printf_yx(stdplane, MENU_START_Y, MENU_START_X,
                          "%s", save_menu_strings[SAVE_NAME_REQUEST]);
        ncplane_printf_yx(stdplane, MENU_START_Y + 2, MENU_START_X,
                          "%s", save_name);
        ncplane_printf_yx(stdplane, MENU_START_Y + 4, MENU_START_X,
                          "%s", save_menu_strings[PRESS_ENTER_CONFIRM]);
        notcurses_render(nc);

        input_event_t input_event;
        if (!get_input_blocking(&input_event)) {
            continue;
        }

        // For text input, we need to handle the raw input value
        uint32_t key_id = input_event.raw_input.id;

        if (input_event.type == INPUT_CONFIRM && name_length > 0) {
            // Enter was pressed and we have a name
            input_active = false;
        } else if (input_event.type == INPUT_CANCEL) {
            // Escape was pressed
            input_active = false;
            name_length = 0;// Set length to 0 to indicate cancellation
        } else if (key_id == NCKEY_BACKSPACE && name_length > 0) {
            // Backspace was pressed and we have characters to delete
            save_name[--name_length] = '\0';
        } else if (key_id != 0 && name_length < 49 &&
                   !(input_event.type == INPUT_UP ||
                     input_event.type == INPUT_DOWN ||
                     input_event.type == INPUT_LEFT ||
                     input_event.type == INPUT_RIGHT)) {
            // A printable character was typed
            save_name[name_length++] = key_id;
            save_name[name_length] = '\0';
        }
    }

    if (name_length > 0) {
        // Store the save name for later use
        strncpy(last_save_name, save_name, sizeof(last_save_name) - 1);
        last_save_name[sizeof(last_save_name) - 1] = '\0';// Ensure null termination

        // Show saving message
        // clear screen
        ncplane_set_channels(stdplane, DEFAULT_COLORS);
        for (uint i = 0; i < ncplane_dim_x(stdplane); i++) {
            for (uint j = 0; j < ncplane_dim_y(stdplane); j++) {
                ncplane_printf_yx(stdplane, (int) j, (int) i, " ");
            }
        }
        ncplane_set_channels(stdplane, DEFAULT_COLORS);
        ncplane_printf_yx(stdplane, MENU_START_Y, MENU_START_X, "%s", save_menu_strings[SAVING]);
        notcurses_render(nc);

        result = MENU_SAVE_GAME;
    }

    return result;
}

menu_result_t show_load_game_menu(bool game_in_progress) {
    menu_result_t result = MENU_CONTINUE;

    if (game_in_progress && !show_confirmation(save_menu_strings[CONFIRM_QUESTION])) {
        // User declined, return to continue
        return MENU_CONTINUE;
    }

    // Create a database connection for the menu to use
    db_connection_t menu_db_connection;
    // TODO: we need to discuss (this is probably reason why the database opens 3 times)
    if (db_open(&menu_db_connection, "../resources/database/game/dungeoncrawl_game.db") != DB_OPEN_STATUS_SUCCESS) {
        log_msg(ERROR, "Menu", "Failed to open database for save file listing");
        return MENU_CONTINUE;
    }

    save_info_container_t* save_infos = get_save_infos(&menu_db_connection);
    if (save_infos == NULL) {
        log_msg(ERROR, "Menu", "Failed to get save files");
        db_close(&menu_db_connection);
        return MENU_CONTINUE;
    }

    if (save_infos->count == 0) {
        // No saves available
        // clear screen
        ncplane_set_channels(stdplane, DEFAULT_COLORS);
        for (uint i = 0; i < ncplane_dim_x(stdplane); i++) {
            for (uint j = 0; j < ncplane_dim_y(stdplane); j++) {
                ncplane_printf_yx(stdplane, (int) j, (int) i, " ");
            }
        }
        ncplane_set_channels(stdplane, DEFAULT_COLORS);
        ncplane_printf_yx(stdplane, MENU_START_Y, MENU_START_X, "%s", save_menu_strings[SAVES_NOT_FOUND]);
        ncplane_printf_yx(stdplane, MENU_START_Y + 2, MENU_START_X, "%s", save_menu_strings[PRESS_ANY_RETURN]);
        notcurses_render(nc);


        db_close(&menu_db_connection);
        free_save_infos(save_infos);
        return MENU_CONTINUE;
    }

    // Display the save files and let the user select one
    int selected_save_index = 0;
    bool selection_active = true;

    while (selection_active) {
        // clear screen
        ncplane_set_channels(stdplane, DEFAULT_COLORS);
        for (uint i = 0; i < ncplane_dim_x(stdplane); i++) {
            for (uint j = 0; j < ncplane_dim_y(stdplane); j++) {
                ncplane_printf_yx(stdplane, (int) j, (int) i, " ");
            }
        }
        ncplane_set_channels(stdplane, DEFAULT_COLORS);
        ncplane_printf_yx(stdplane, MENU_START_Y, MENU_START_X, "%s", save_menu_strings[SELECT_SAVE]);

        int y = MENU_START_Y + 2;
        for (int i = 0; i < save_infos->count; i++) {
            char save_info[MAX_STRING_LENGTH + TIMESTAMP_LENGTH + 3];
            snprintf(save_info, sizeof(save_info), "%s (%s)", save_infos->infos[i].name, save_infos->infos[i].timestamp);

            if (i == selected_save_index) {
                ncplane_set_channels(stdplane, DEFAULT_COLORS);
                ncplane_printf_yx(stdplane, y, MENU_START_X, "%s", save_info);
            } else {
                ncplane_set_channels(stdplane, DEFAULT_COLORS);
                ncplane_printf_yx(stdplane, y, MENU_START_X, "%s", save_info);
            }
            y += MENU_ITEM_SPACING;
        }

        ncplane_set_channels(stdplane, DEFAULT_COLORS);
        ncplane_printf_yx(stdplane, y + 2, MENU_START_X, "%s", save_menu_strings[NAVIGATE_INSTRUCTIONS]);
        notcurses_render(nc);

        input_event_t input_event;
        if (!get_input_blocking(&input_event)) {
            continue;
        }

        // Use our logical input types
        switch (input_event.type) {
            case INPUT_UP:
                selected_save_index = (selected_save_index - 1 + save_infos->count) % save_infos->count;
                break;
            case INPUT_DOWN:
                selected_save_index = (selected_save_index + 1) % save_infos->count;
                break;
            case INPUT_CONFIRM:
                // Set the selected save file ID for loading
                result = MENU_LOAD_GAME;
                selected_save_file_id = save_infos->infos[selected_save_index].id;
                selection_active = false;
                break;
            case INPUT_CANCEL:
                selection_active = false;
                break;
        }
    }

    // Clean up the save files
    free_save_infos(save_infos);
    db_close(&menu_db_connection);

    return result;
}

void shutdown_save_menu(void) {
    if (save_menu_strings != NULL) {
        for (int i = 0; i < MAX_SAVE_MENU_STRINGS; i++) {
            if (save_menu_strings[i] != NULL) {
                free(save_menu_strings[i]);
            }
        }
        free(save_menu_strings);
    }
}
