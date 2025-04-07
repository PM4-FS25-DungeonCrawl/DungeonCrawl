#ifndef COMBAT_MODE_H
#define COMBAT_MODE_H

// === INCLUDE ===
#include "character_stats.h"

#include <stdbool.h>
#include <stdio.h>

// === ENUM ===
typedef enum {
    MENU_COMBAT,
          MENU_ABILITY,
    MENU_ITEM
} combat_state;

// === FUNCTION DECLARATIONS ===
bool combat(player_t* player, monster_t* monster);
combat_state combat_menu(player_t* player, monster_t* monster);
bool ability_menu(player_t* player, monster_t* monster);
bool item_menu(player_t* player, monster_t* monster);
int use_ability(character_t* attacker, character_t* defender, ability_t* ability);
bool roll_hit(ability_t* ability, character_t* defender);
int roll_damage(ability_t* ability);
int roll_dice(dice_size_t dice_size);
int deal_damage(int damage, damage_type_t damage_type, character_t* character);
void use_item(player_t* player, monster_t* monster, UsableItem* item);
int print_combat_view(character_t* player, character_t* monster, bool red_monster_sprite);
ability_t* get_random_ability(character_t* character);
void display_enemy_attack_message(player_t* player, monster_t* monster, int damage_dealt);
void display_item_message(player_t* player, monster_t* monster, UsableItem* item);
void display_combat_message(player_t* player, monster_t* monster, const char* message);

#endif// COMBAT_MODE_H
