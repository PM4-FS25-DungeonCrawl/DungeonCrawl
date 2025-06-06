/**
 * @file save_menu.c
 * @brief Implementation of the save menu.
 */
#include "save_menu.h"

#include "../common.h"
#include "../database/database.h"
#include "../database/game/gamestate_database.h"
#include "../io/input/input_handler.h"
#include "../io/output/common/output_handler.h"
#include "../io/output/common/text_output.h"
#include "../local/local_handler.h"
#include "local/save_menu_local.h"
#include "src/menu/menu.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>


int selected_save_file_id = -1;
char last_save_name[50] = {0};

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
    menu_result_t result = MENU_CONTINUE;

    // Get save name from user using the output handler
    bool confirmed = get_text_input(
            save_menu_strings[SAVE_NAME_REQUEST],
            save_name,
            sizeof(save_name),
            save_menu_strings[PRESS_ENTER_CONFIRM],
            MENU_START_Y,
            MENU_START_X);

    if (confirmed) {
        // Store the save name for later use
        strncpy(last_save_name, save_name, sizeof(last_save_name) - 1);
        last_save_name[sizeof(last_save_name) - 1] = '\0';// Ensure null termination

        // Show saving message
        show_message_screen(
                save_menu_strings[SAVING],
                NULL,
                MENU_START_Y,
                MENU_START_X);

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

    // Use the global database connection from game.h instead of creating a new one
    extern db_connection_t db_connection;

    save_info_container_t* save_infos = get_save_infos(&db_connection);
    if (save_infos == NULL) {
        log_msg(ERROR, "Menu", "Failed to get save files");
        return MENU_CONTINUE;
    }

    if (save_infos->count == 0) {
        // No saves available - show message and return
        show_message_screen(
                save_menu_strings[SAVES_NOT_FOUND],
                save_menu_strings[PRESS_ANY_RETURN],
                MENU_START_Y,
                MENU_START_X);

        free_save_infos(save_infos);
        return MENU_CONTINUE;
    }

    // Prepare the save file options for the menu
    const char** save_options = malloc(save_infos->count * sizeof(char*));
    if (!save_options) {
        log_msg(ERROR, "Menu", "Failed to allocate memory for save options");
        free_save_infos(save_infos);
        return MENU_CONTINUE;
    }

    // Format save info strings for the menu
    for (int i = 0; i < save_infos->count; i++) {
        save_options[i] = malloc(MAX_STRING_LENGTH + TIMESTAMP_LENGTH + 5);
        if (!save_options[i]) {
            // Clean up previously allocated memory
            for (int j = 0; j < i; j++) {
                free((void*) save_options[j]);
            }
            free(save_options);
            free_save_infos(save_infos);
            log_msg(ERROR, "Menu", "Failed to allocate memory for save option");
            return MENU_CONTINUE;
        }
        snprintf((char*) save_options[i], MAX_STRING_LENGTH + TIMESTAMP_LENGTH + 5,
                 "%s (%s)", save_infos->infos[i].name, save_infos->infos[i].timestamp);
    }

    // Display the save files and let the user select one
    int selected_save_index = 0;
    bool selection_active = true;

    while (selection_active) {
        // Clear the screen
        clear_screen();

        // Print the title and options
        print_text_default(MENU_START_Y, MENU_START_X, save_menu_strings[SELECT_SAVE]);

        // Print the save options with highlighting
        for (int i = 0; i < save_infos->count; i++) {
            if (i == selected_save_index) {
                // Highlight selected option with inverted colors
                print_text(MENU_START_Y + 2 + (i * MENU_ITEM_SPACING),
                           MENU_START_X,
                           save_options[i],
                           INVERTED_COLORS);
            } else {
                // Normal option
                print_text_default(MENU_START_Y + 2 + (i * MENU_ITEM_SPACING),
                                   MENU_START_X,
                                   save_options[i]);
            }
        }

        // Print the navigation instructions
        print_text_default(MENU_START_Y + 2 + (save_infos->count * MENU_ITEM_SPACING) + 2,
                           MENU_START_X,
                           save_menu_strings[NAVIGATE_INSTRUCTIONS]);

        // Render the frame
        render_frame();

        // Get input
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
            default:
                break;
        }
    }

    // Clean up resources
    for (int i = 0; i < save_infos->count; i++) {
        free((void*) save_options[i]);
    }
    free(save_options);
    free_save_infos(save_infos);

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
