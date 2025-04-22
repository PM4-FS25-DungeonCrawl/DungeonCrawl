#include "game.h"
#include "main.h"

#include "../include/termbox2.h"
#include "character/character.h"
#include "character/monster.h"
#include "character/player.h"
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

#include <stdbool.h>
#include <stdio.h>
#include <time.h>

db_connection_t db_connection;
bool game_in_progress;
game_state_t current_state;

int init_game() {
    if (tb_init() != 0) {
        log_msg(ERROR, "Game", "Failed to initialize termbox");
        return FAIL_TB_INIT;
    }
    tb_set_output_mode(TB_OUTPUT_NORMAL);

    // seeding random function
    srand(time(NULL));

    // Initialize database connection
    if (db_open(&db_connection, "resources/database/game/dungeoncrawl_game.db") != DB_OPEN_STATUS_SUCCESS) {
        log_msg(ERROR, "Game", "Failed to open database");
        return 1;
    }
    create_tables_game_state(&db_connection); // only for dungeoncrawl_game.db

    bool running = true;          //should only be set in the state machine
    int exit_code = 0;

    game_in_progress = false;// Flag to track if a game has been started
    current_state = MAIN_MENU;


    if (init_local() == 0) {
        current_state = EXIT_WITH_ERROR;
        exit_code = FAIL_LOCAL_INIT;
    }
    init_map_mode();
    init_main_menu();
    init_combat_mode();


    ability_table_t* ability_table = init_ability_table();
    character_t* goblin = create_new_goblin();//initialize standard goblin
    character_t* player = create_new_player();//initialize blank player
    potion_t* healing_potion = init_potion("Healing Potion", HEALING, 20);

    //TODO: this needs to be refactored!!!!!

    if (ability_table == NULL || goblin == NULL || player == NULL || healing_potion == NULL) {
        log_msg(ERROR, "Game", "Failed to initialize game components");
        current_state = EXIT_WITH_ERROR;
        exit_code = FAIL_GAME_ENTITY_INIT;
    } else if (current_state != EXIT_WITH_ERROR) {
        // add abilities to player and goblin
        add_ability(goblin, &ability_table->abilities[BITE]);
        add_ability(player, &ability_table->abilities[FIREBALL]);
        add_ability(player, &ability_table->abilities[SWORD_SLASH]);
        //add healing potion to player
        add_potion(player, healing_potion);
        log_msg(INFO, "Game", "game loop starting");
    }

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
            game_in_progress = true;// Mark that a game is now in progress
            tb_clear();             // Clear screen before generating map
            current_state = GENERATE_MAP;
            break;
        case MENU_CONTINUE:
            tb_clear();// Clear screen before map mode
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
                load_success = get_game_state_by_id(&db_connection, save_id, map, revealed_map, WIDTH, HEIGHT, set_player_start_pos);
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
            tb_clear();// Clear screen before generating new floor
            current_state = GENERATE_MAP;
            break;
        case COMBAT:
            log_msg(INFO, "Game", "Entering combat mode");
            current_state = COMBAT_MODE;
            break;
        case SHOW_MENU:
            tb_clear();// Clear the screen before showing menu
            current_state = MAIN_MENU;
            break;
        default:
            log_msg(ERROR, "game", "Unknown return value from map_mode_update");
    }
}


