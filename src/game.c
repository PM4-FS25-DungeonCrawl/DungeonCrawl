#include "game.h"

#include "character/character.h"
#include "combat/combat_mode.h"
#include "database/database.h"
#include "database/gamestate/gamestate_database.h"
#include "game_data.h"
#include "inventory/inventory_mode.h"
#include "logging/logger.h"
#include "map/map.h"
#include "map/map_generator.h"
#include "map/map_mode.h"
#include "menu/main_menu.h"
#include "menu/save_menu.h"
#include "src/common.h"
#include "stats/stats_mode.h"

#include <locale.h>
#include <notcurses/notcurses.h>
#include <stdbool.h>
#include <stdio.h>

// Global notcurses instance and standard plane
struct notcurses* nc = NULL;
struct ncplane* stdplane = NULL;


db_connection_t db_connection;
bool game_in_progress;
game_state_t current_state;
int exit_code;

void game_loop();

void combat_mode_state();

void run_game() {
    // TODO: remove after notcurses switch
    setlocale(LC_ALL, "");

    // Initialize notcurses
    notcurses_options ncopt;
    memset(&ncopt, 0, sizeof(ncopt));
    nc = notcurses_init(&ncopt, stdout);
    if (nc == NULL) {
        log_msg(ERROR, "game", "failed to initialize notcurses");
        return;
    }
    stdplane = notcurses_stdplane(nc);
    ncplane_set_bg_rgb(stdplane, 0x281D10);
    game_in_progress = false;// Flag to track if a game has been started
    current_state = MAIN_MENU;
    //start the game loop
    game_loop();
}

void game_loop() {
    bool running = true;//should only be set in the state machine

    while (running) {
        switch (current_state) {
            case MAIN_MENU:
                main_menu_state();
                break;
            case GENERATE_MAP:
                generate_map();
                current_state = MAP_MODE;
                break;
            case MAP_MODE:
                map_mode_state();
                break;
            case COMBAT_MODE:
                reset_goblin();
                combat_mode_state();
                break;
            case LOOT_MODE:
                loot_mode_state();
                break;
            case INVENTORY_MODE:
                inventory_mode_state();
                break;
            case STATS_MODE:
                stats_mode(player);// Pass your player object

                ncplane_set_channels(stdplane, DEFAULT_COLORS);
                for (uint i = 0; i < ncplane_dim_x(stdplane); i++) {
                    for (uint j = 0; j < ncplane_dim_y(stdplane); j++) {
                        ncplane_printf_yx(stdplane, (int) j, (int) i, " ");
                    }
                }
                current_state = MAP_MODE;
                break;


            case EXIT:
                running = false;
                break;
        }
    }
    // Close database connection
    db_close(&db_connection);
    shutdown_combat_mode();
    shutdown_logger();

    // Shutdown notcurses
    if (nc) {
        notcurses_stop(nc);
        nc = NULL;
        stdplane = NULL;
    }
}

void main_menu_state() {
    switch (show_main_menu(game_in_progress)) {
        case MENU_START_GAME:
            log_msg(INFO, "Game", "Starting new game");
            game_in_progress = true;// Mark that a game is now in progress
            // clear screen
            ncplane_set_channels(stdplane, DEFAULT_COLORS);
            for (uint i = 0; i < ncplane_dim_x(stdplane); i++) {
                for (uint j = 0; j < ncplane_dim_y(stdplane); j++) {
                    ncplane_printf_yx(stdplane, (int) j, (int) i, " ");
                }
            }
            current_state = GENERATE_MAP;
            break;
        case MENU_CONTINUE:
            // clear screen
            ncplane_set_channels(stdplane, DEFAULT_COLORS);
            for (uint i = 0; i < ncplane_dim_x(stdplane); i++) {
                for (uint j = 0; j < ncplane_dim_y(stdplane); j++) {
                    ncplane_printf_yx(stdplane, (int) j, (int) i, " ");
                }
            }
            current_state = MAP_MODE;
            break;
        case MENU_SAVE_GAME:
            log_msg(INFO, "Game", "Saving game state to database");

            // Get the save name from the menu
            const char* save_name = get_save_name();
            if (save_name == NULL) {
                save_name = "Unnamed Save";// Default name if none provided
            }

            // Save the game with the provided name
            save_game_state(&db_connection, map, revealed_map, WIDTH, HEIGHT, get_player_pos(), save_name);
            log_msg(INFO, "Game", "Game state saved as '%s'", save_name);

            // clear screen
            ncplane_set_channels(stdplane, DEFAULT_COLORS);
            for (uint i = 0; i < ncplane_dim_x(stdplane); i++) {
                for (uint j = 0; j < ncplane_dim_y(stdplane); j++) {
                    ncplane_printf_yx(stdplane, (int) j, (int) i, " ");
                }
            }
            current_state = MAP_MODE;
            break;
        case MENU_LOAD_GAME:
            log_msg(INFO, "Game", "Loading game state from database");

            const int save_id = get_selected_save_file_id();
            bool load_success = false;

            if (save_id != -1) {
                // Load the selected save file
                log_msg(INFO, "Game", "Loading save file ID: %d", save_id);
                load_success = get_game_state_by_id(&db_connection, save_id, map, revealed_map, WIDTH, HEIGHT,
                                                    set_player_start_pos);
            } else {
                // No save file was selected, try loading the latest save
                log_msg(INFO, "Game", "No save ID provided, loading most recent save");
                load_success = get_game_state(&db_connection, map, revealed_map, WIDTH, HEIGHT, set_player_start_pos);
            }

            if (load_success) {
                // Set game_in_progress flag
                game_in_progress = true;

                log_msg(INFO, "Game", "Game state loaded successfully");
                // clear screen
                ncplane_set_channels(stdplane, DEFAULT_COLORS);
                for (uint i = 0; i < ncplane_dim_x(stdplane); i++) {
                    for (uint j = 0; j < ncplane_dim_y(stdplane); j++) {
                        ncplane_printf_yx(stdplane, (int) j, (int) i, " ");
                    }
                }
                current_state = MAP_MODE;
            } else {
                log_msg(ERROR, "Game", "Failed to load game state - generating new map");
                // clear screen
                ncplane_set_channels(stdplane, DEFAULT_COLORS);
                for (uint i = 0; i < ncplane_dim_x(stdplane); i++) {
                    for (uint j = 0; j < ncplane_dim_y(stdplane); j++) {
                        ncplane_printf_yx(stdplane, (int) j, (int) i, " ");
                    }
                }
                current_state = GENERATE_MAP;
            }
            break;
        case MENU_CHANGE_LANGUAGE:
            current_state = MAIN_MENU;
            break;
        case MENU_EXIT:
            current_state = EXIT;
            break;
    }
}

void map_mode_state() {
    switch (map_mode_update(player)) {
        case CONTINUE:
            break;
        case QUIT:
            current_state = EXIT;
            break;
        case NEXT_FLOOR:
            // clear screen
            ncplane_set_channels(stdplane, DEFAULT_COLORS);
            for (uint i = 0; i < ncplane_dim_x(stdplane); i++) {
                for (uint j = 0; j < ncplane_dim_y(stdplane); j++) {
                    ncplane_printf_yx(stdplane, (int) j, (int) i, " ");
                }
            }
            reset_current_stats(player);// Heal player before entering new floor
            current_state = GENERATE_MAP;
            break;
        case COMBAT:
            log_msg(INFO, "Game", "Entering combat mode");
            current_state = COMBAT_MODE;
            break;
        case SHOW_INVENTORY:
            log_msg(INFO, "Game", "Entering inventory mode");
            current_state = INVENTORY_MODE;
            break;
        case SHOW_MENU:
            // clear screen
            ncplane_set_channels(stdplane, DEFAULT_COLORS);
            for (uint i = 0; i < ncplane_dim_x(stdplane); i++) {
                for (uint j = 0; j < ncplane_dim_y(stdplane); j++) {
                    ncplane_printf_yx(stdplane, (int) j, (int) i, " ");
                }
            }
            current_state = MAIN_MENU;
            break;
        case SHOW_STATS:
            // clear screen
            for (uint i = 0; i < ncplane_dim_x(stdplane); i++) {
                for (uint j = 0; j < ncplane_dim_y(stdplane); j++) {
                    ncplane_printf_yx(stdplane, (int) j, (int) i, " ");
                }
            }
            current_state = STATS_MODE;
        default:
            log_msg(ERROR, "game", "Unknown return value from map_mode_update");
    }
}

void combat_mode_state() {
    switch (start_combat(player, goblin)) {
        case PLAYER_WON:
            log_msg(FINE, "Game", "Player won the combat");
            current_state = LOOT_MODE;
            // TODO: add loot to player
            // TODO: delete goblin from map
            // clear screen
            ncplane_set_channels(stdplane, DEFAULT_COLORS);
            for (uint i = 0; i < ncplane_dim_x(stdplane); i++) {
                for (uint j = 0; j < ncplane_dim_y(stdplane); j++) {
                    ncplane_printf_yx(stdplane, (int) j, (int) i, " ");
                }
            }
            log_msg(INFO, "Game", "Entering loot mode");
            current_state = LOOT_MODE;
            break;
        case PLAYER_LOST:
            log_msg(FINE, "Game", "Player lost the combat");
            //TODO: instead of exiting the game, a death screen should be shown
            current_state = EXIT;
            break;
        case EXIT_GAME:
            current_state = EXIT;
            break;
    }
}

void loot_mode_state() {
    start_inventory_mode(player, goblin);
    current_state = MAP_MODE;
}

void inventory_mode_state() {
    start_inventory_mode(player, NULL);
    current_state = MAP_MODE;
}
