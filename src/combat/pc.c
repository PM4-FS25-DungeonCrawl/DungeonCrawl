#include <stdlib.h>
#include <stdio.h>

#include "pc.h"
#include "../logging/logger.h"

player_t* init_player(const char *name, const int health, const int armor, const int might, const int deflection, const int fortitude, const int will) {
    player_t* player = malloc(sizeof(player_t));
    if (player == NULL) {
        // memory for player could not be allocated
        log_msg(ERROR, "Player", "Failed to allocate memory for player: %s", name);
        return NULL;
    }

    character_t* base = init_character(PLAYER, name, health, armor, might, deflection, fortitude, will);
    if (base == NULL) {
        // memory for character failed to allocate
        free(player);
        return NULL;
    }
    player->base = base;
    player->item_count = 0;

    return player;
}

void free_player(player_t* player) {
    if (player != NULL) {
        free_character(player->base);

        for (int i = 0; i < MAX_ITEMS; i++) {
            if (player->usable_items[i] != NULL) {
                free(player->usable_items[i]);
            }
            if (player->equipable_items[i] != NULL) {
                free(player->equipable_items[i]);
            }
        }

        free(player);
    }
}

void add_item_to_player(player_t* player, usable_item_t* item) {
    if (player->item_count < MAX_ITEMS) {
        player->usable_items[player->item_count] = item;
        player->item_count++;
    } else {
        log_msg(INFO, "Player", "Inventory is full! Cannot add item: %s", item->name);
    }
}

void add_item_to_player(player_t* player, equipable_item_t* item) {
    if (player->item_count < MAX_ITEMS) {
        player->equipable_items[player->item_count] = item;
        player->item_count++;
    } else {
        log_msg(INFO, "Player", "Inventory is full! Cannot add item: %s", item->name);
    }
}
