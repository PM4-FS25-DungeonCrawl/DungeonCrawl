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
void use_ability(Player *player, Monster *monster, Ability *ability);
bool roll_hit(Player *player, Ability *ability, Monster *monster);
int roll_damage(Ability *ability);
int roll_dice(DiceSize dice_size);
void deal_damage(int damage, Monster *monster);
void take_damage(Monster *monster, Player *player);
void use_item();
int print_combat_view(Player *player, Monster *monster);

#endif // COMBAT_MODE_H