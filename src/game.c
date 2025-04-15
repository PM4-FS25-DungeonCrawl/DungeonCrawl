#include "game.h"

#include "../include/termbox2.h"
#include "character/character.h"
#include "character/monster.h"
#include "character/player.h"
#include "combat/ability.h"
#include "combat/combat_mode.h"
#include "combat/damage.h"
#include "database/database.h"
#include "database/gamestate/gamestate_database.h"
#include "item/gear.h"
#include "item/potion.h"
#include "logging/logger.h"
#include "map/map.h"
#include "map/map_generator.h"
#include "map/map_mode.h"
#include "menu/main_menu.h"

#include <stdio.h>
#include <time.h>
#include <stdbool.h>

typedef enum {
    MAIN_MENU,
    MAP_MODE,
    COMBAT_MODE,
    GENERATE_MAP,
    EXIT
} game_state_t;

int init_game() {
    if (tb_init() != 0) {
        log_msg(ERROR, "Game", "Failed to initialize termbox");
        return 1;
    }
    tb_set_output_mode(TB_OUTPUT_NORMAL);

    // seeding random function
    srand(time(NULL));

    // Initialize database connection
    DBConnection db_connection;
    if (db_open(&db_connection, "resources/database/game/dungeoncrawl_game.db") != 0) {
        log_msg(ERROR, "Game", "Failed to open database");
        return 1;
    }

    bool running = true;//should only be set in the state machine
    bool game_in_progress = false; // Flag to track if a game has been started
    game_state_t current_state = MAIN_MENU;

    init_map_mode();
    init_main_menu();

    ability_table_t* ability_table = init_ability_table();
    character_t* goblin = create_new_goblin();//initialize standard goblin
    character_t* player = create_new_player();//initialize blank player
    potion_t* healing_potion = init_potion("Healing Potion", HEALING, 20);

    if (ability_table == NULL || goblin == NULL || player == NULL || healing_potion == NULL) {
        log_msg(ERROR, "Game", "Failed to initialize game components");
        current_state = EXIT;
    } else {
        // add abilities to player and goblin
        add_ability(goblin, &ability_table->abilities[BITE]);
        log_msg(INFO, "Game", "Added ability: %s to goblin", goblin->abilities[0]->name);
        add_ability(player, &ability_table->abilities[FIREBALL]);
        log_msg(INFO, "Game", "Added ability: %s to player", player->abilities[0]->name);
        add_ability(player, &ability_table->abilities[SWORD_SLASH]);
        log_msg(INFO, "Game", "Added ability: %s to player", player->abilities[1]->name);
        //add healing potion to player
        add_potion(player, healing_potion);
        log_msg(INFO, "Game", "game loop starting");
    }

    while (running) {
        switch (current_state) {
            case MAIN_MENU:
                switch (show_main_menu(game_in_progress)) {
                    case MENU_START_GAME:
                        log_msg(INFO, "Game", "Starting new game");
                        // Reset the map to prepare for generation
                        for (int y = 0; y < HEIGHT; y++) {
                            for (int x = 0; x < WIDTH; x++) {
                                map[x][y] = WALL;
                                revealed_map[x][y] = HIDDEN;
                            }
                        }
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
                        int player_x, player_y;
                        get_player_pos(&player_x, &player_y);
                        save_game_state(&db_connection, WIDTH, HEIGHT, (int (*)[HEIGHT])map, (int (*)[HEIGHT])revealed_map, player_x, player_y);
                        log_msg(INFO, "Game", "Game state saved successfully");
                        tb_clear();
                        current_state = MAP_MODE;
                        break;
                    case MENU_LOAD_GAME:
                        log_msg(INFO, "Game", "Loading game state from database");
                        int width, height;
                        int load_player_x, load_player_y;
                        int* loaded_map;
                        int* loaded_revealed_map;
                        
                        if (get_game_state(&db_connection, &width, &height, &loaded_map, &loaded_revealed_map, &load_player_x, &load_player_y)) {
                            // Copy loaded data to the game's map arrays
                            for (int y = 0; y < height; y++) {
                                for (int x = 0; x < width; x++) {
                                    map[x][y] = loaded_map[y * width + x];
                                    revealed_map[x][y] = loaded_revealed_map[y * width + x];
                                }
                            }
                            
                            // Set player position
                            set_player_start_pos(load_player_x, load_player_y);
                            
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
                break;
            case GENERATE_MAP:
                generate_map();
                current_state = MAP_MODE;
                break;
            case MAP_MODE:
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
            case EXIT:
                free_ability_table(ability_table);
                free_character(goblin);
                free_character(player);
                free_potion(healing_potion);
                running = false;
                break;
        }
    }

    // Close database connection
    db_close(&db_connection);
    
    shutdown_logger();
    tb_shutdown();
    return 0;
}
