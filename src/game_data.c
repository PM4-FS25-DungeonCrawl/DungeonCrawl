#include "game_data.h"

#include <stddef.h>

#include "character/monster.h"
#include "character/player.h"

ability_table_t *ability_table;
character_t *goblin;
character_t *player;
potion_t *healing_potion;


int init_game_data() {
    ability_table = init_ability_table();
    goblin = create_new_goblin(); //initialize standard goblin
    player = create_new_player(); //initialize blank player
    healing_potion = init_potion("Healing Potion", HEALING, 20);

    if (ability_table == NULL || goblin == NULL || player == NULL || healing_potion == NULL) {
        return 1;
    } else {
        // add abilities to player and goblin
        add_ability(goblin, &ability_table->abilities[BITE]);
        add_ability(player, &ability_table->abilities[FIREBALL]);
        add_ability(player, &ability_table->abilities[SWORD_SLASH]);
        //add healing potion to player
        add_potion(player, healing_potion);
    }
    return 0;
}
