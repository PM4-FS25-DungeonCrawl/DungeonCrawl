#include "game_content.h"
#include "character/player.h"
#include "character/monster.h"
#include "logging/logger.h"
#include <stddef.h>

// Global variables for game entities
character_t* player = NULL;
character_t* goblin = NULL;
ability_table_t* ability_table = NULL;
potion_t* healing_potion = NULL;

void init_game_content(void) {
    // Initialize abilities
    ability_table = init_ability_table();
    
    // Initialize characters
    goblin = create_new_goblin();  // Initialize standard goblin
    player = create_new_player();  // Initialize blank player
    
    // Initialize items
    healing_potion = init_potion("Healing Potion", HEALING, 20);
    
    if (ability_table == NULL || goblin == NULL || player == NULL || healing_potion == NULL) {
        log_msg(ERROR, "Game Content", "Failed to initialize game components");
        return;
    }
    
    // Add abilities to player and goblin
    add_ability(goblin, &ability_table->abilities[BITE]);
    add_ability(player, &ability_table->abilities[FIREBALL]);
    add_ability(player, &ability_table->abilities[SWORD_SLASH]);
    
    // Add healing potion to player
    add_potion(player, healing_potion);
}

void free_game_content(void) {
    if (ability_table != NULL) {
        free_ability_table(ability_table);
        ability_table = NULL;
    }
    
    if (goblin != NULL) {
        free_character(goblin);
        goblin = NULL;
    }
    
    if (player != NULL) {
        free_character(player);
        player = NULL;
    }
    
    if (healing_potion != NULL) {
        free_potion(healing_potion);
        healing_potion = NULL;
    }
}
