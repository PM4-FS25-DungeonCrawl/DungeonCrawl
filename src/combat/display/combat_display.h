#ifndef COMBAT_DISPLAY_H
#define COMBAT_DISPLAY_H

#include "../../character/character_fw.h"
#include "../../character/character.h"
#include "../../item/usable_item.h"
#include "../ability.h"
#include "../../../include/termbox2.h"

int display_combat_view(const character_t* player, const character_t* monster, bool red_monster_sprite);
void display_combat_message(const character_t* player, const character_t* monster, const char *message, bool red_monster_sprite);
void display_ability_options(int y, int selected_index, int option_count, const ability_t* abilities[]);
void display_item_options(int y, int selected_index, int option_count, const item_t* items[]);
void display_item_message(const character_t* player, const character_t* monster, usable_item_t* item);
void display_attack_message(const character_t* attacker, const character_t* target, const ability_t* ability, int damage_dealt);
void display_missed_message(const character_t* player, const character_t* monster, const ability_t* ability);

#endif //COMBAT_DISPLAY_H