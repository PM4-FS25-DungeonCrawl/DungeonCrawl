#ifndef COMBAT_MODE_H
#define COMBAT_MODE_H

// === INCLUDE ===
#include <stdio.h>
#include <stdbool.h>
#include "character_stats.h"

// === ENUM ===
typedef enum {
    MENU_COMBAT,
    MENU_ABILITY,
    MENU_ITEM
} combat_state;

// === FUNCTION DECLARATIONS ===
bool combat(player *player, monster *monster);
combat_state combat_menu(player *player, monster *monster);
bool ability_menu(player *player, monster *monster);
bool item_menu(player *player, monster *monster);
int use_ability(character *attacker, character *defender, ability *ability);
bool roll_hit(ability *ability, character *defender);
int roll_damage(ability *ability);
int roll_dice(dice_size dice_size);
int deal_damage(int damage, damage_type damage_type, character *character);
void use_item(player *player,monster *monster, UsableItem *item);
int print_combat_view(character *player, character *monster, bool red_monster_sprite);
ability *get_random_ability(character *character);
void display_enemy_attack_message(player *player, monster *monster, int damage_dealt);
void display_item_message(player *player, monster *monster, UsableItem *item);
void display_combat_message(player *player, monster *monster, const char *message);

#endif // COMBAT_MODE_H
