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
void ability_menu(Player *player, Monster *monster);
void item_menu(Player *player, Monster *monster);
void use_ability(Character *attacker, Character *defender, Ability *ability);
bool roll_hit(Ability *ability, Character *defender);
int roll_damage(Ability *ability);
int roll_dice(DiceSize dice_size);
void deal_damage(int damage, DamageType damage_type, Character *character);
void use_item(Player *player, int index);
int print_combat_view(Character *player, Character *monster);
Ability *get_random_ability(Character *character);

#endif // COMBAT_MODE_H