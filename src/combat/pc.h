#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"
#include "../items/item.h"

#define MAX_ITEMS 10

typedef struct {
    character_t* base;

    // player inventory -> "two" inventories for each item type
    // but the total number of items should still be limited to MAX_ITEMS
    usable_item_t* usable_items[MAX_ITEMS];
    equipable_item_t* equipable_items[MAX_ITEMS];
    int item_count;
} player_t;

player_t* init_player(const char *name, int health, int armor, int might, int deflection, int fortitude, int will);
void free_player(player_t* player);

void add_usable_item_to_player(player_t *player, usable_item_t *item);
void add_equipable_item_to_player(player_t *player, equipable_item_t *item);

#endif //PLAYER_H
