#ifndef COMBAT_MODE_H
#define COMBAT_MODE_H

// === INCLUDE ===
#include <stdio.h>
#include <stdbool.h>
#include "character_stats.h"
#include "../include/termbox2.h"

// === ENUM ===
typedef enum {
    MENU_COMBAT,
    MENU_ABILITY,
    MENU_ITEM
} combat_state;

// === FUNCTION DECLARATIONS ===
bool combat(Player *player, Monster *monster);
combat_state combat_menu(Player *player, Monster *monster);
bool ability_menu(Player *player, Monster *monster);
bool item_menu(Player *player, Monster *monster);
int use_ability(Character *attacker, Character *defender, Ability *ability);
bool roll_hit(Ability *ability, Character *defender);
int roll_damage(Ability *ability);
int roll_dice(DiceSize dice_size);
int deal_damage(int damage, DamageType damage_type, Character *character);
void use_item(Player *player,Monster *monster, UsableItem *item);
int print_combat_view(Character *player, Character *monster, bool red_monster_sprite);
Ability *get_random_ability(Character *character);
void display_enemy_attack_message(Player *player, Monster *monster, int damage_dealt);
void display_item_message(Player *player, Monster *monster, UsableItem *item);
void display_combat_message(Player *player, Monster *monster, const char *message);

#endif // COMBAT_MODE_H