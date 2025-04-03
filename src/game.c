#include <stdio.h>
#include <stdbool.h>

#include "game.h"

#include "combat/ability.h"
#include "combat/npc.h"
#include "combat/pc.h"
#include "combat/combat_mode.h"

#include "map/map_mode.h"
#include "map/map_generator.h"
#include "../include/termbox2.h"
#include "logging/logger.h"

typedef enum {
    MAIN_MENU,
    MAP_MODE,
    COMBAT_MODE,
    GENERATE_MAP,
    EXIT
} game_state_t;

int init_game(){
    if (tb_init() != 0) {
        log_msg(ERROR, "Game", "Failed to initialize termbox");
        return 1;
    }
    tb_set_output_mode(TB_OUTPUT_NORMAL);

    bool running = true; //should only be set in the state machine
    game_state_t current_state = GENERATE_MAP;

    init_map_mode();

    ability_table_t* ability_table = init_ability_table();
    monster_t* goblin = init_goblin(); //initialize standard goblin
    player_t* player = init_player("Hero", 100, 10, 5, 5, 5, 5);
    usable_item_t* healingPotion = init_usable_item("Healing Potion", HEALING, 20);
    if (ability_table == NULL || goblin == NULL || player == NULL || healingPotion == NULL) {
        log_msg(ERROR, "Game", "Failed to initialize game components");
        current_state = EXIT;
    } else {
        // add abilities to player and goblin
        add_ability_to_character(goblin->base, &ability_table->table[BITE]);
        add_ability_to_character(player->base, &ability_table->table[FIREBALL]);
        add_ability_to_character(player->base, &ability_table->table[SWORD_SLASH]);

        //add healing potion to player
        add_usable_item_to_player(player, healingPotion);
        log_msg(INFO, "Game", "game loop starting");
    }

    while (running) {
        switch (current_state) {
            case MAIN_MENU:
                break;
            case GENERATE_MAP:
                generate_map();
                current_state = MAP_MODE;
                break;
            case MAP_MODE:
                // TODO: should change the state of the state machine, when walking into a monster
                if (map_mode_update()) {
                    current_state = EXIT;
                }
                break;
            case COMBAT_MODE: {
                switch (start_combat(player, goblin)) {
                    case PLAYER_WON:
                        log_msg(FINE, "Game", "Player won the combat");
                        // TODO: add loot to player
                        // TODO: delete goblin from map
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
            }
            case EXIT:
                free_ability_table(ability_table);
                free_goblin(goblin);
                free_player(player);
                close_log_file(1);
                running = false;
                break;
        }
    }

    tb_shutdown();
    return 0;
}
