#ifndef DRAW_COMBAT_MODE_H
#define DRAW_COMBAT_MODE_H

#include "../../../include/termbox2.h"
#include "../../character/character.h"
#include "../../character/character_fw.h"
#include "../../item/potion.h"
#include "../ability.h"

int draw_combat_view(const character_t* attacker, const character_t* target, bool red_target_sprite);
int draw_enemy(int y, bool red_target_sprite);
void draw_potion_message(const character_t* attacker, const character_t* target, potion_t* potion);
void draw_combat_message(const character_t* attacker, const character_t* target, const char* message, bool red_target_sprite);
void draw_attack_message(const character_t* attacker, const character_t* target, const ability_t* ability, int damage_dealt);
void draw_missed_message(const character_t* attacker, const character_t* target, const ability_t* ability);
void draw_oom_message(const character_t* attacker, const character_t* target, const ability_t* ability);
void draw_ability_options(int y, int selected_index, const ability_t* abilities[]);
void draw_potion_options(int y, int selected_index, const potion_t* potions[]);
#endif//DRAW_COMBAT_MODE_H