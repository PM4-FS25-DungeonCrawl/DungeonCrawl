/**
 * @file game.c
 * @brief Implements the main game loop and state handling for DungeonCrawl.
 */
#include "game.h"

#include "character/character.h"
#include "combat/combat_mode.h"
#include "database/database.h"
#include "database/game/character_database.h"
#include "database/game/gamestate_database.h"
#include "game_data.h"
#include "inventory/inventory_mode.h"
#include "io/input/input_types.h"
#include "io/io_handler.h"
#include "io/output/common/output_handler.h"
#include "io/output/common/text_output.h"
#include "logging/logger.h"
#include "map/map.h"
#include "map/map_generator.h"
#include "map/map_mode.h"
#include "menu/main_menu.h"
#include "menu/save_menu.h"
#include "src/common.h"
#include "stats/stats_mode.h"

#include <locale.h>
#include <stdbool.h>
#include <stdio.h>

db_connection_t db_connection;
bool game_in_progress;
game_state_t current_state;
int exit_code;

/**
 * @brief The main game loop of the application.
 */
void game_loop();

/**
 * @brief The state machine for the combat mode. 
 */
void combat_mode_state();
/**
 * @brief Handles the stats mode state of the game.
 *
 * This function manages the interactions and transitions that occur
 * within the stats mode, including displaying player stats and
 * managing skill points.
 */
void stats_mode_state();

/**
 * @brief Load the a game from the database.
 *
 * @param game_state_id The id of the game state to be loaded.
 * @param setter A setter for the player position.
 * @return 0 on success, 1 if the player could not be propperly reset, 2 if game state could not be loaded, 3 if player is NULL
 */
int loading_game(int game_state_id, player_pos_setter_t setter);

void run_game() {
    game_in_progress = false;// Flag to track if a game has been started

    current_state = MAIN_MENU;
    //start the game loop
    game_loop();
}

void game_loop() {
    bool running = true;//should only be set in the state machine

    while (running) {
        // Process current game state
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
                combat_mode_state();
                break;

            case LOOT_MODE:
                loot_mode_state();
                break;

            case INVENTORY_MODE:
                inventory_mode_state();
                break;

            case STATS_MODE:
                stats_mode_state();
                break;

            case EXIT:
                running = false;
                break;
        }
    }

    // Close database connection
    db_close(&db_connection);
}

void main_menu_state() {
    switch (show_main_menu(game_in_progress)) {
        case MENU_START_GAME: {
            // Prompt for player name
            char player_name[MAX_NAME_LENGTH];
            if (prompt_player_name(player_name)) {
                init_player(player_name);
                game_in_progress = true;// Mark that a game is now in progress
                current_floor = 1;
                clear_screen();
                current_state = GENERATE_MAP;
            } else {
                // Player canceled name input, stay in menu
                current_state = MAIN_MENU;
            }
            break;
        }
        case MENU_CONTINUE:
            clear_screen();
            current_state = MAP_MODE;
            break;
        case MENU_SAVE_GAME: {
            // Get the save name from the menu
            const char* save_name = get_save_name();
            if (save_name == NULL) {
                // Use player name as default if no save name provided
                save_name = get_player_name();
            }

            // Save the game with the provided name
            const sqlite_int64 game_state_id = save_game_state(&db_connection, map, revealed_map, WIDTH, HEIGHT, current_floor, get_player_pos(), save_name);
            save_character(&db_connection, *player, game_state_id);

            clear_screen();
            current_state = MAP_MODE;
            break;
        }
        case MENU_LOAD_GAME: {
            const int save_id = get_selected_save_file_id() != -1 ? get_selected_save_file_id() : get_latest_save_id(&db_connection);
            const int load_status = loading_game(save_id, set_player_start_pos);
            switch (load_status) {
                case 0:
                    log_msg(INFO, "Game", "Game loaded successfully");
                    // Set game_in_progress flag
                    game_in_progress = true;
                    clear_screen();
                    current_state = MAP_MODE;
                    break;
                case 1:
                    log_msg(ERROR, "Game", "Failed to reset player character - generating new map");
                    clear_screen();
                    current_state = GENERATE_MAP;
                    break;
                case 2:
                    log_msg(ERROR, "Game", "Failed to load game state - generating new map");
                    clear_screen();
                    current_state = GENERATE_MAP;
                    break;
                case 3:
                    log_msg(ERROR, "Game", "Failed to load player character - generating new map");
                    clear_screen();
                    current_state = GENERATE_MAP;
                    break;
                default:
                    log_msg(ERROR, "Game", "Unknown error loading game state");
                    clear_screen();
                    current_state = GENERATE_MAP;
                    break;
            }
            break;
        }
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
            clear_screen();
            reset_player_stats(player);// Heal player before entering new floor
            current_state = GENERATE_MAP;
            break;
        case COMBAT:
            current_state = COMBAT_MODE;
            break;
        case SHOW_INVENTORY:
            current_state = INVENTORY_MODE;
            break;
        case SHOW_MENU:
            clear_screen();
            current_state = MAIN_MENU;
            break;
        case SHOW_STATS:
            clear_screen();
            current_state = STATS_MODE;
            break;
        default:
            log_msg(ERROR, "game", "Unknown return value from map_mode_update");
    }
}

/**
 * @brief The state machine of the combat mode.
 */
void combat_mode_state() {
    switch (start_combat(player, goblin)) {
        case CONTINUE_COMBAT:
            break;
        case PLAYER_WON:
            clear_screen();
            current_state = LOOT_MODE;
            break;
        case PLAYER_LOST:
            //TODO: instead of exiting the game, a death screen should be shown
            current_state = EXIT;
            break;
        case EXIT_GAME:
            current_state = EXIT;
            break;
    }
}

void loot_mode_state() {
    switch (start_inventory(player, goblin)) {
        case CONTINUE_INVENTORY:
            break;
        case EXIT_TO_MAP:
            reset_goblin();
            current_state = MAP_MODE;
            break;
    }
}

void inventory_mode_state() {
    switch (start_inventory(player, NULL)) {
        case CONTINUE_INVENTORY:
            break;
        case EXIT_TO_MAP:
            current_state = MAP_MODE;
            break;
    }
}

/**
 * @brief The state machine for the stats mode.
 */
void stats_mode_state() {
    switch (stats_mode(player)) {
        case STATS_WINDOW:
            break;
        case STATS_EXIT:
            current_state = MAP_MODE;
            break;
    }
}

int loading_game(const int game_state_id, const player_pos_setter_t setter) {
    if (reset_player() != 0) return 1;
    int* return_floor = &current_floor;
    if (get_game_state_by_id(&db_connection, game_state_id, map, revealed_map, WIDTH, HEIGHT, return_floor, setter) != 1) return 2;
    current_floor = *return_floor;
    reset_goblin();
    get_character_from_db(&db_connection, player, game_state_id);
    if (player == NULL) return 3;
    return 0;
}
