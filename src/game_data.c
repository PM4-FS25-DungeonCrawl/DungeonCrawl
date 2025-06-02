/**
 * @file game_data.c
 * @brief Implementation for game_data.
 */
#include "game_data.h"

#include "character/monster.h"
#include "character/player.h"
#include "common.h"
#include "game.h"
#include "item/loot_generation.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

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
    player = create_new_player(main_memory_pool);
    reset_goblin();

    if (ability_table == NULL || potion_table == NULL || gear_table == NULL || player == NULL) return 1;

    add_potion(goblin, &potion_table->potions[HEALING]);

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
    generate_loot(goblin, gear_table, potion_table, 1);
    return 0;
}

int init_player(char* name) {
    free_character(main_memory_pool, player);
    player = create_new_player(main_memory_pool);
    if (player == NULL) {
        return 1;
    }
    player->base_attack = &ability_table->abilities[PUNCH];
    add_ability(player, player->base_attack);
    add_potion(player, &potion_table->potions[HEALING]);
    add_potion(player, &potion_table->potions[MANA]);
    add_potion(player, &potion_table->potions[STAMINA]);
    equip_gear(player, gear_table->gears[ARMING_SWORD]);
    if (name != NULL) {
        snprintf(player->name, sizeof(player->name), "%s", name);
    } else {
        snprintf(player->name, sizeof(player->name), "Ash Ketchup");
    }
    return 0;
}

int reset_player() {
    free_character(main_memory_pool, player);
    player = create_new_player(main_memory_pool);
    if (player == NULL) {
        return 1;
    }
    player->base_attack = &ability_table->abilities[PUNCH];
    add_ability(player, player->base_attack);
    return 0;
}

const char* get_player_name(void) {
    if (player == NULL || strlen(player->name) == 0) {
        return "Ash Ketchup";
    }
    return player->name;
}
