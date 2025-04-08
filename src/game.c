#include <stdio.h>

#include "game.h"

#include "character/character.h"
#include "character/player.h"
#include "character/monster.h"

#include "item/equipable_item.h"
#include "item/usable_item.h"

#include "combat/ability.h"
#include "combat/combat_mode.h"
#include "combat/damage.h"

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
    game_state_t current_state = COMBAT_MODE;

    init_map_mode();

    ability_table_t* ability_table = init_ability_table();
    character_t * goblin = create_new_goblin(); //initialize standard goblin
    character_t * player = create_new_player(); //initialize blank player
    usable_item_t* healing_potion = init_usable_item("Healing Potion", HEALING, 20);

    if (ability_table == NULL || goblin == NULL || player == NULL || healing_potion == NULL) {
        log_msg(ERROR, "Game", "Failed to initialize game components");
        current_state = EXIT;
    } else {
        // add abilities to player and goblin
        add_ability(goblin, &ability_table->table[BITE]);
        add_ability(player, &ability_table->table[FIREBALL]);
        add_ability(player, &ability_table->table[SWORD_SLASH]);

        //add healing potion to player
        add_item(player, healing_potion->base);
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
                switch (map_mode_update()) {
                    case CONTINUE:
                        break;
                    case QUIT:
                        current_state = EXIT;
                        break;
                    case NEXT_FLOOR:
                        current_state = GENERATE_MAP;
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
                free_usable_item(healing_potion);
                close_log_file(1);
                running = false;
                break;
        }
    }

    tb_shutdown();
    return 0;
}
