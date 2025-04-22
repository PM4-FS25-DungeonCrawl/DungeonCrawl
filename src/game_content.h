#ifndef GAME_CONTENT_H
#define GAME_CONTENT_H

#include "character/character.h"
#include "combat/ability.h"
#include "item/potion.h"

// Global variables for game entities
extern character_t* player;
extern character_t* goblin;
extern ability_table_t* ability_table;
extern potion_t* healing_potion;

// Initialize all game content (player, monsters, abilities, items)
void init_game_content(void);

// Free all game content resources
void free_game_content(void);

#endif // GAME_CONTENT_H