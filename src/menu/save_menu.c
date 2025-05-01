#include "save_menu.h"

#include "../../include/termbox2.h"
#include "../common.h"
#include "../database/database.h"
#include "../database/game/gamestate_database.h"
#include "../local/local.h"
#include "../local/local_strings.h"
#include "main_menu.h"

#include <stdio.h>
#include <string.h>

// === Internal Functions ===
void update_save_menu_local(void);

// Global variables to store menu state
int selected_save_file_id = -1;
char last_save_name[50] = {0};

void init_save_menu() {
    // update local once, so the strings are initialized
    update_save_menu_local();
    // add update local function to the observer list
    add_local_observer(update_save_menu_local);
}


int get_selected_save_file_id(void) {
    return selected_save_file_id;
}

const char* get_save_name(void) {
    log_msg(INFO, "Save Menu", "Getting save name: %s", last_save_name[0] == '\0' ? "NULL" : last_save_name);
    if (last_save_name[0] == '\0') {
        return NULL;
    }
    return last_save_name;
}

menu_result_t show_save_game_menu(void) {
    log_msg(INFO, "Save Menu", "Entering show_save_game_menu");
    char save_name[50] = {0};
    int name_length = 0;
    bool input_active = true;
    menu_result_t result = MENU_CONTINUE;

    // Get save name from user
    while (input_active && name_length < 49) {
        tb_clear();
        tb_print(MENU_START_X, MENU_START_Y, TB_WHITE, TB_DEFAULT, "Enter name for save file:");
        tb_print(MENU_START_X, MENU_START_Y + 2, TB_WHITE, TB_DEFAULT, save_name);
        tb_print(MENU_START_X, MENU_START_Y + 4, TB_WHITE, TB_DEFAULT, "Press Enter when done");
        tb_present();

        struct tb_event input_event;
        int ret = tb_poll_event(&input_event);


        switch (input_event.key) {
            case TB_KEY_ENTER:
                if (name_length > 0) {
                    input_active = false;
                }
                break;
            case TB_KEY_BACKSPACE2:
            case TB_KEY_BACKSPACE:
                // Handle both the standard TB_KEY_BACKSPACE (0x08) and TB_KEY_BACKSPACE2 (0x7f)
                if (name_length > 0) {
                    save_name[--name_length] = '\0';
                }
                break;
            case TB_KEY_ESC:
                // Cancel save
                input_active = false;
                name_length = 0;// Set length to 0 to indicate cancellation
                break;
            default:
                if (input_event.ch != 0 && name_length < 49) {
                    save_name[name_length++] = input_event.ch;
                    save_name[name_length] = '\0';
                }
                break;
        }
    }

    if (name_length > 0) {
        // Store the save name for later use
        strncpy(last_save_name, save_name, sizeof(last_save_name) - 1);
        last_save_name[sizeof(last_save_name) - 1] = '\0';// Ensure null termination

        // Show saving message
        tb_clear();
        tb_print(MENU_START_X, MENU_START_Y, TB_WHITE, TB_DEFAULT, "Saving game...");
        tb_present();

        log_msg(INFO, "Menu", "Saving game with name: %s", save_name);

        result = MENU_SAVE_GAME;
    }

    return result;
}

menu_result_t show_load_game_menu(bool game_in_progress) {
    menu_result_t result = MENU_CONTINUE;

    if (game_in_progress && !show_confirmation("Do you want to continue?")) {
        // User declined, return to continue
        return MENU_CONTINUE;
    }

    // Create a database connection for the menu to use
    db_connection_t menu_db_connection;
    // TODO: we need to discuss (this is probably reason why the database opens 3 times)
    if (db_open(&menu_db_connection, "resources/database/game/dungeoncrawl_game.db") != DB_OPEN_STATUS_SUCCESS) {
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
        tb_clear();
        tb_print(MENU_START_X, MENU_START_Y, TB_WHITE, TB_DEFAULT, "No saved games found.");
        tb_print(MENU_START_X, MENU_START_Y + 2, TB_WHITE, TB_DEFAULT, "Press any key to return to the menu.");
        tb_present();

        struct tb_event tb_event;
        tb_poll_event(&tb_event);

        db_close(&menu_db_connection);
        free_save_infos(save_infos);
        return MENU_CONTINUE;
    }

    // Display the save files and let the user select one
    int selected_save_index = 0;
    bool selection_active = true;

    while (selection_active) {
        tb_clear();
        tb_print(MENU_START_X, MENU_START_Y, TB_WHITE, TB_DEFAULT, "Select a save file:");

        int y = MENU_START_Y + 2;
        for (int i = 0; i < save_infos->count; i++) {
            char save_info[MAX_STRING_LENGTH + TIMESTAMP_LENGTH + 3];
            snprintf(save_info, sizeof(save_info), "%s (%s)", save_infos->infos[i].name, save_infos->infos[i].timestamp);

            if (i == selected_save_index) {
                tb_print(MENU_START_X, y, TB_BLACK, TB_WHITE, save_info);
            } else {
                tb_print(MENU_START_X, y, TB_WHITE, TB_DEFAULT, save_info);
            }
            y += MENU_ITEM_SPACING;
        }

        tb_print(MENU_START_X, y + 2, TB_WHITE, TB_DEFAULT, "Arrow keys: Navigate | Enter: Select | Esc: Back");
        tb_present();

        struct tb_event tb_event;
        tb_poll_event(&tb_event);

        switch (tb_event.key) {
            case TB_KEY_ARROW_UP:
                selected_save_index = (selected_save_index - 1 + save_infos->count) % save_infos->count;
                break;
            case TB_KEY_ARROW_DOWN:
                selected_save_index = (selected_save_index + 1) % save_infos->count;
                break;
            case TB_KEY_ENTER:
                // Set the selected save file ID for loading
                result = MENU_LOAD_GAME;
                selected_save_file_id = save_infos->infos[selected_save_index].id;
                selection_active = false;
                break;
            case TB_KEY_ESC:
                selection_active = false;
                break;
        }
    }

    // Clean up the save files
    free_save_infos(save_infos);
    db_close(&menu_db_connection);

    return result;
}

void update_save_menu_local(void) {
}
