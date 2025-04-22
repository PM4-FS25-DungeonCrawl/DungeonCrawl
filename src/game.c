#include "game.h"

#include "../include/termbox2.h"
#include "character/character.h"
#include "combat/ability.h"
#include "combat/combat_mode.h"
#include "database/database.h"
#include "database/gamestate/gamestate_database.h"
#include "item/potion.h"
#include "local/local.h"
#include "logging/logger.h"
#include "map/map.h"
#include "map/map_generator.h"
#include "map/map_mode.h"
#include "menu/main_menu.h"
#include "menu/save_menu.h"
#include "game_data.h"

#include <stdbool.h>
#include <stdio.h>
#include <time.h>

bool game_in_progress;
game_state_t current_state;

int run_game() {
    // TODO: remove after notcurses switch
    if (tb_init() != 0) {
        log_msg(ERROR, "Game", "Failed to initialize termbox");
        return FAIL_TB_INIT;
    }
    tb_set_output_mode(TB_OUTPUT_NORMAL);

    bool running = true; //should only be set in the state machine
    int exit_code = 0;

    game_in_progress = false; // Flag to track if a game has been started
    current_state = MAIN_MENU;

    //TODO: this needs to be refactored!!!!!

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
                switch (start_combat(player, goblin)) {
                    case PLAYER_WON:
                        log_msg(FINE, "Game", "Player won the combat");
                    // TODO: add loot to player
                    // TODO: delete goblin from map
                        tb_clear();
                        current_state = MAP_MODE;
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
                break;
            case EXIT: //TODO: shouldn't exit code be set here?
                running = false;
                break;
            case EXIT_WITH_ERROR: //TODO: shouldn't exit code be set here?
                running = false;
                break;
        }
    }
    free_ability_table(ability_table);
    free_character(goblin);
    free_character(player);
    free_potion(healing_potion);
    shutdown_local();
    // Close database connection
    db_close(&db_connection);
    shutdown_combat_mode();
    shutdown_logger();
    tb_shutdown();
    return exit_code;
}

void main_menu_state() {
    switch (show_main_menu(game_in_progress)) {
        case MENU_START_GAME:
            log_msg(INFO, "Game", "Starting new game");
            game_in_progress = true; // Mark that a game is now in progress
            tb_clear(); // Clear screen before generating map
            current_state = GENERATE_MAP;
            break;
        case MENU_CONTINUE:
            tb_clear(); // Clear screen before map mode
            current_state = MAP_MODE;
            break;
        case MENU_SAVE_GAME:
            log_msg(INFO, "Game", "Saving game state to database");

        // Get the save name from the menu
            const char *save_name = get_save_name();
            if (save_name == NULL) {
                save_name = "Unnamed Save"; // Default name if none provided
            }

        // Save the game with the provided name
            save_game_state(&db_connection, map, revealed_map, WIDTH, HEIGHT, get_player_pos(), save_name);
            log_msg(INFO, "Game", "Game state saved as '%s'", save_name);

            tb_clear();
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
                tb_clear();
                current_state = MAP_MODE;
            } else {
                log_msg(ERROR, "Game", "Failed to load game state - generating new map");
                tb_clear();
                current_state = GENERATE_MAP;
            }
            break;
        case MENU_EXIT:
            current_state = EXIT;
            break;
    }
}

void map_mode_state() {
    switch (map_mode_update()) {
        case CONTINUE:
            break;
        case QUIT:
            current_state = EXIT;
            break;
        case NEXT_FLOOR:
            tb_clear(); // Clear screen before generating new floor
            current_state = GENERATE_MAP;
            break;
        case COMBAT:
            log_msg(INFO, "Game", "Entering combat mode");
            current_state = COMBAT_MODE;
            break;
        case SHOW_MENU:
            tb_clear(); // Clear the screen before showing menu
            current_state = MAIN_MENU;
            break;
        default:
            log_msg(ERROR, "game", "Unknown return value from map_mode_update");
    }
}
