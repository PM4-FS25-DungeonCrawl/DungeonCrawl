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


int init_game_data() {
    ability_table = init_ability_table(main_memory_pool, &db_connection);
    potion_table = init_potion_table(main_memory_pool, &db_connection);
    gear_table = init_gear_table(main_memory_pool, &db_connection, ability_table);
    player = create_new_player(main_memory_pool);//initialize blank player

    reset_goblin();

    if (ability_table == NULL || potion_table == NULL || gear_table == NULL || player == NULL) return 1;
    add_potion(player, &potion_table->potions[HEALING]);
    add_potion(player, &potion_table->potions[MANA]);
    add_potion(player, &potion_table->potions[STAMINA]);

    add_gear(player, gear_table->gears[LONGSWORD]);
    equip_gear(player, player->gear_inventory[0]);

    add_gear(player, gear_table->gears[IRON_HELM_OF_THE_BOAR]);
    equip_gear(player, player->gear_inventory[0]);

    add_gear(player, gear_table->gears[BATTLEPLATE_OF_THE_BOAR]);
    equip_gear(player, player->gear_inventory[0]);

    add_gear(player, gear_table->gears[STEEL_SABATONS_OF_THE_BOAR]);
    equip_gear(player, player->gear_inventory[0]);

    return 0;
}

int free_game_data() {
    free_ability_table(main_memory_pool, ability_table);
    free_potion_table(main_memory_pool, potion_table);
    free_gear_table(main_memory_pool, gear_table);
    free_character(main_memory_pool, player);
    free_character(main_memory_pool, goblin);
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