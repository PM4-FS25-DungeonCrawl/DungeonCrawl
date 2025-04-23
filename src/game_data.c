#include "game_data.h"

#include "character/monster.h"
#include "character/player.h"

#include <stddef.h>

ability_table_t* ability_table;
character_t* goblin;
character_t* player;
potion_t* healing_potion;

/**
 * Reset the goblin's state
 * @return 0 if successful, 1 if failed
 */
int reset_goblin() {
    goblin = create_new_goblin();
    if (goblin == NULL) {
        return 1;
    } else {
        add_ability(goblin, &ability_table->abilities[BITE]);
    }
    return 0;
}


/**
 * Initialize game data
 * @return 0 if successful, 1 if failed
 */
int init_game_data() {
    ability_table = init_ability_table();
    player = create_new_player();//initialize blank player
    healing_potion = init_potion("Healing Potion", HEALING, 20);

    reset_goblin();

    if (ability_table == NULL || player == NULL || healing_potion == NULL) {
        return 1;
    } else {
        // add abilities to player
        add_ability(player, &ability_table->abilities[FIREBALL]);
        add_ability(player, &ability_table->abilities[SWORD_SLASH]);
        //add healing potion to player
        add_potion(player, healing_potion);
    }
    return 0;
}

/**
 * Free game data
 */
int free_game_data() {
    free_ability_table(ability_table);
    free_character(player);
    free_character(goblin);
    free_potion(healing_potion);

    return 0;
}
