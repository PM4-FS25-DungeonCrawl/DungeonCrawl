#include "main_menu.h"

#include "../database/database.h"
#include "../logging/logger.h"

#include <notcurses/notcurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>// For nanosleep
//
#ifdef __APPLE__
    #define KEY_EVENT NCTYPE_PRESS
#else
    #define KEY_EVENT NCTYPE_UNKNOWN
#endif /* ifdef __APPLE__ */


// External reference to notcurses context
extern struct notcurses* nc;
extern struct ncplane* stdplane;

// Global variables to store menu state
static int selected_save_file_id = -1;
static char last_save_name[50] = {0};

// Menu options
#define NEW_GAME_OPTION "New Game"
#define CONTINUE_OPTION "Continue"
#define SAVE_GAME_OPTION "Save Game"
#define LOAD_GAME_OPTION "Load Game"
#define EXIT_OPTION "Exit"

// Menu constants
#define TITLE "DUNGEON CRAWL"
#define MENU_START_Y 5
#define MENU_START_X 10
#define MENU_ITEM_SPACING 2

// Color channels
#define RED_ON_BLACK NCCHANNELS_INITIALIZER(255, 0, 0, 0, 0, 0)
#define WHITE_ON_BLACK NCCHANNELS_INITIALIZER(255, 255, 255, 0, 0, 0)
#define BLACK_ON_WHITE NCCHANNELS_INITIALIZER(0, 0, 0, 255, 255, 255)

void init_main_menu(void) {
    log_msg(INFO, "Menu", "Initializing main menu");
}

static void draw_menu(const char** menu_options, int menu_count, int selected_index) {
    // Draw title
    ncplane_set_channels(stdplane, RED_ON_BLACK);
    ncplane_putstr_yx(stdplane, MENU_START_Y, MENU_START_X, TITLE);

    // Draw menu options
    int y = MENU_START_Y + 3;
    for (int i = 0; i < menu_count; i++) {
        if (i == selected_index) {
            ncplane_set_channels(stdplane, BLACK_ON_WHITE);
        } else {
            ncplane_set_channels(stdplane, WHITE_ON_BLACK);
        }
        ncplane_putstr_yx(stdplane, y, MENU_START_X, menu_options[i]);
        y += MENU_ITEM_SPACING;
    }

    notcurses_render(nc);
}

static bool show_confirmation(const char* message) {
    ncplane_set_channels(stdplane, WHITE_ON_BLACK);
    ncplane_putstr_yx(stdplane, MENU_START_Y, MENU_START_X, "Warning: All unsaved progress will be lost!");
    ncplane_putstr_yx(stdplane, MENU_START_Y + 2, MENU_START_X, message);
    ncplane_putstr_yx(stdplane, MENU_START_Y + 4, MENU_START_X, "(Y/N)");
    notcurses_render(nc);

    while (1) {
        ncinput input;
        notcurses_get_blocking(nc, &input);

        // Only process press events
        if (input.evtype == NCTYPE_UNKNOWN || input.evtype == NCTYPE_PRESS) {
            if (input.id == 'y' || input.id == 'Y') {
                return true;
            } else if (input.id == 'n' || input.id == 'N') {
                return false;
            }
        }
    }
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

menu_result_t show_main_menu(bool game_in_progress) {
    const char* menu_options[5];
    int menu_count;

    // Reset the selected save file ID
    selected_save_file_id = -1;

    // Always include New Game
    menu_options[0] = NEW_GAME_OPTION;

    if (game_in_progress) {
        // If game is in progress, show all options
        menu_options[1] = CONTINUE_OPTION;
        menu_options[2] = SAVE_GAME_OPTION;
        menu_options[3] = LOAD_GAME_OPTION;
        menu_options[4] = EXIT_OPTION;
        menu_count = 5;
    } else {
        // If no game in progress, only show New Game, Load Game and Exit
        menu_options[1] = LOAD_GAME_OPTION;
        menu_options[2] = EXIT_OPTION;
        menu_count = 3;
    }

    int selected_index = 0;
    menu_result_t result = MENU_CONTINUE;
    bool menu_active = true;

    while (menu_active) {
        draw_menu(menu_options, menu_count, selected_index);

        ncinput input;
        memset(&input, 0, sizeof(input));
        uint32_t ret = notcurses_get_nblock(nc, &input);


        if (ret > 0) {
            // Only process press events
            if (input.evtype == NCTYPE_UNKNOWN || input.evtype == NCTYPE_PRESS) {
                if (input.id == NCKEY_UP) {
                    // Move up
                    selected_index = (selected_index - 1 + menu_count) % menu_count;
                } else if (input.id == NCKEY_DOWN) {
                    // Move down
                    selected_index = (selected_index + 1) % menu_count;
                } else if (input.id == NCKEY_ENTER) {
                    if (strcmp(menu_options[selected_index], NEW_GAME_OPTION) == 0) {
                        if (game_in_progress && !show_confirmation("Do you want to continue?")) {
                            // User declined, continue menu loop
                            continue;
                        }
                        result = MENU_START_GAME;
                        menu_active = false;
                    } else if (strcmp(menu_options[selected_index], CONTINUE_OPTION) == 0) {
                        result = MENU_CONTINUE;
                        menu_active = false;
                    } else if (strcmp(menu_options[selected_index], SAVE_GAME_OPTION) == 0) {
                        char save_name[50] = {0};
                        int name_length = 0;
                        bool input_active = true;

                        // Get save name from user
                        while (input_active && name_length < 49) {
                            ncplane_set_channels(stdplane, WHITE_ON_BLACK);
                            ncplane_putstr_yx(stdplane, MENU_START_Y, MENU_START_X, "Enter name for save file:");
                            ncplane_putstr_yx(stdplane, MENU_START_Y + 2, MENU_START_X, save_name);
                            ncplane_putstr_yx(stdplane, MENU_START_Y + 4, MENU_START_X, "Press Enter when done");
                            notcurses_render(nc);

                            ncinput input_event;
                            notcurses_get_blocking(nc, &input_event);

                            // Only process press events
                            if (input.evtype == NCTYPE_UNKNOWN || input.evtype == NCTYPE_PRESS) {
                                if (input_event.id == NCKEY_ENTER && name_length > 0) {
                                    input_active = false;
                                } else if (input_event.id == NCKEY_BACKSPACE && name_length > 0) {
                                    // Handle backspace
                                    save_name[--name_length] = '\0';
                                } else if (input_event.id == NCKEY_ESC) {
                                    // Cancel save
                                    input_active = false;
                                    name_length = 0;// Set length to 0 to indicate cancellation
                                    break;
                                } else if (input_event.id > 0 && input_event.id < 127 && name_length < 49) {
                                    // Only accept printable ASCII
                                    save_name[name_length++] = input_event.id;
                                    save_name[name_length] = '\0';
                                }
                            }
                        }

                        if (name_length > 0) {
                            // Store the save name for later use
                            strncpy(last_save_name, save_name, sizeof(last_save_name) - 1);
                            last_save_name[sizeof(last_save_name) - 1] = '\0';// Ensure null termination

                            // Show saving message
                            ncplane_set_channels(stdplane, WHITE_ON_BLACK);
                            ncplane_putstr_yx(stdplane, MENU_START_Y, MENU_START_X, "Saving game...");
                            notcurses_render(nc);

                            log_msg(INFO, "Menu", "Saving game with name: %s", save_name);

                            result = MENU_SAVE_GAME;
                            menu_active = false;
                        } else {
                            // If user pressed ESC, just stay in the menu
                            continue;
                        }
                    } else if (strcmp(menu_options[selected_index], LOAD_GAME_OPTION) == 0) {
                        if (game_in_progress && !show_confirmation("Do you want to continue?")) {
                            // User declined, continue menu loop
                            continue;
                        }

                        // Create a database connection for the menu to use
                        db_connection_t menu_db_connection;
                        if (!db_open(&menu_db_connection, "resources/database/game/dungeoncrawl_game.db")) {
                            log_msg(ERROR, "Menu", "Failed to open database for save file listing");
                            continue;
                        }

                        save_info_container_t* save_infos = get_save_infos(&menu_db_connection);
                        if (save_infos == NULL) {
                            log_msg(ERROR, "Menu", "Failed to get save files");
                            db_close(&menu_db_connection);
                            continue;
                        }

                        if (save_infos->count == 0) {
                            // No saves available
                            ncplane_set_channels(stdplane, WHITE_ON_BLACK);
                            ncplane_putstr_yx(stdplane, MENU_START_Y, MENU_START_X, "No saved games found.");
                            ncplane_putstr_yx(stdplane, MENU_START_Y + 2, MENU_START_X, "Press any key to return to the menu.");
                            notcurses_render(nc);

                            ncinput input;
                            notcurses_get_blocking(nc, &input);

                            //commented out for testing if you find this, yeet it
                            // // Drain any additional queued events
                            // ncinput discard;
                            // while (notcurses_get_nblock(nc, &discard) > 0) {
                            //     // Discard extra events
                            // }

                            db_close(&menu_db_connection);
                            free_save_infos(save_infos);
                            continue;
                        }

                        // Display the save files and let the user select one
                        int selected_save_index = 0;
                        bool selection_active = true;

                        while (selection_active) {
                            ncplane_set_channels(stdplane, WHITE_ON_BLACK);
                            ncplane_putstr_yx(stdplane, MENU_START_Y, MENU_START_X, "Select a save file:");

                            int y = MENU_START_Y + 2;
                            for (int i = 0; i < save_infos->count; i++) {
                                char save_info[MAX_STRING_LENGTH + TIMESTAMP_LENGTH + 3];
                                snprintf(save_info, sizeof(save_info), "%s (%s)", save_infos->infos[i].name, save_infos->infos[i].timestamp);

                                if (i == selected_save_index) {
                                    ncplane_set_channels(stdplane, BLACK_ON_WHITE);
                                    ncplane_putstr_yx(stdplane, y, MENU_START_X, save_info);
                                } else {
                                    ncplane_set_channels(stdplane, WHITE_ON_BLACK);
                                    ncplane_putstr_yx(stdplane, y, MENU_START_X, save_info);
                                }
                                y += MENU_ITEM_SPACING;
                            }

                            ncplane_set_channels(stdplane, WHITE_ON_BLACK);
                            ncplane_putstr_yx(stdplane, y + 2, MENU_START_X, "Arrow keys: Navigate | Enter: Select | Esc: Back");
                            notcurses_render(nc);

                            ncinput nc_event;
                            notcurses_get_blocking(nc, &nc_event);

                            //commented out for testing if you find this, yeet it
                            // // Drain any additional queued events
                            // ncinput discard;
                            // while (notcurses_get_nblock(nc, &discard) > 0) {
                            //     // Discard extra events
                            // }

                            // Only process press events
                            if (input.evtype == NCTYPE_UNKNOWN || input.evtype == NCTYPE_PRESS) {
                                if (nc_event.id == NCKEY_UP) {
                                    selected_save_index = (selected_save_index - 1 + save_infos->count) % save_infos->count;
                                } else if (nc_event.id == NCKEY_DOWN) {
                                    selected_save_index = (selected_save_index + 1) % save_infos->count;
                                } else if (nc_event.id == NCKEY_ENTER) {
                                    // Set the selected save file ID for loading
                                    result = MENU_LOAD_GAME;
                                    selected_save_file_id = save_infos->infos[selected_save_index].id;
                                    selection_active = false;
                                    menu_active = false;
                                } else if (nc_event.id == NCKEY_ESC) {
                                    selection_active = false;
                                }
                            }
                        }

                        // Clean up the save files
                        free_save_infos(save_infos);
                        db_close(&menu_db_connection);
                    } else if (strcmp(menu_options[selected_index], EXIT_OPTION) == 0) {
                        if (game_in_progress && !show_confirmation("Do you want to exit?")) {
                            // User declined, continue menu loop
                            continue;
                        }

                        result = MENU_EXIT;
                        menu_active = false;
                    }
                } else if (input.id == NCKEY_ESC || (input.id == 'c' && (input.modifiers & NCKEY_MOD_CTRL))) {
                    result = MENU_EXIT;
                    menu_active = false;
                }
            }
        }
    }
    return result;
}
