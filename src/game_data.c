#include "game_data.h"

#include "character/monster.h"
#include "character/player.h"
#include "game.h"
#include "common.h"

#include <stddef.h>

// === External Global Variables ===
ability_table_t* ability_table;
potion_table_t* potion_table;
gear_table_t* gear_table;
character_t* goblin;
character_t* player;
potion_t* healing_potion;
potion_t* mana_potion;
gear_t* sword;


int init_game_data() {
    ability_table = init_ability_table(main_memory_pool, &db_connection);
    potion_table = init_potion_table(main_memory_pool, &db_connection);
    gear_table = init_gear_table(main_memory_pool, &db_connection, ability_table);
    player = create_new_player(main_memory_pool);//initialize blank player
    healing_potion = init_potion(main_memory_pool, "Healing Potion", HEALING, 20);
    mana_potion = init_potion(main_memory_pool, "Mana Potion", MANA_REGEN, 20);

    reset_goblin();

    if (ability_table == NULL || player == NULL || healing_potion == NULL) return 1;
    // add abilities to player
    add_ability(player, &ability_table->abilities[FIREBLAST]);
    add_ability(player, &ability_table->abilities[FIREBALL]);
    add_ability(player, &ability_table->abilities[PYROBLAST]);
    add_ability(player, &ability_table->abilities[MANA_SHIELD]);
    //add healing potion to a player
    add_potion(player, healing_potion);
    add_potion(player, mana_potion);
    add_potion(player, &potion_table->potions[HEALING]);
    add_gear(player, gear_table->gears[1]);
    equip_gear(player, gear_table->gears[1]);
    return 0;
}

int free_game_data() {
    free_ability_table(main_memory_pool, ability_table);
    free_potion_table(main_memory_pool, potion_table);
    //free_gear_table(main_memory_pool, gear_table);
    free_character(main_memory_pool, player);
    free_character(main_memory_pool, goblin);
    free_potion(main_memory_pool, healing_potion);
    free_potion(main_memory_pool, mana_potion);
    return 0;
}

int reset_goblin() {
    memory_pool_free(main_memory_pool, goblin);
    goblin = create_new_goblin(main_memory_pool);
    if (goblin == NULL) {
        return 1;
    }
    add_ability(goblin, &ability_table->abilities[BITE]);
    return 0;
}