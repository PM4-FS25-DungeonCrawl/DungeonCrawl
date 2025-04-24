#include "game_data.h"

#include "character/monster.h"
#include "character/player.h"
#include "common.h"

#include <stddef.h>

// === Internal Global Variables ===
memory_pool_t* mem_pool;

// === External Global Variables ===
ability_table_t* ability_table;
character_t* goblin;
character_t* player;
potion_t* healing_potion;

int init_game_data(memory_pool_t* memory_pool) {
    NULL_PTR_HANDLER_RETURN(memory_pool, 1, "Game Data", "Memory pool is NULL");
    mem_pool = memory_pool;

    ability_table = init_ability_table(mem_pool);
    player = create_new_player(mem_pool);//initialize blank player
    healing_potion = init_potion(mem_pool, "Healing Potion", HEALING, 20);

    reset_goblin();

    if (ability_table == NULL || player == NULL || healing_potion == NULL) return 1;
    // add abilities to player
    add_ability(player, &ability_table->abilities[FIREBALL]);
    add_ability(player, &ability_table->abilities[SWORD_SLASH]);
    //add healing potion to a player
    add_potion(player, healing_potion);
    return 0;
}

int free_game_data() {
    free_ability_table(mem_pool, ability_table);
    free_character(mem_pool, player);
    free_character(mem_pool, goblin);
    free_potion(mem_pool, healing_potion);
    return 0;
}

int reset_goblin() {
    NULL_PTR_HANDLER_RETURN(mem_pool, 1, "Game Data", "Game Data was not initialized");
    memory_pool_free(mem_pool, goblin);
    goblin = create_new_goblin(mem_pool);
    if (goblin == NULL) {
        return 1;
    }
    add_ability(goblin, &ability_table->abilities[BITE]);
    return 0;
}