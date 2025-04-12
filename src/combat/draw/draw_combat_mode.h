#ifndef DRAW_COMBAT_MODE_H
#define DRAW_COMBAT_MODE_H

#include "../../../include/termbox2.h"
#include "../../character/character.h"
#include "../../character/character_fw.h"
#include "../../common.h"

vector2d_t draw_combat_view(const character_t* player, const character_t* enemy, char* enemy_sprite, bool red_target_sprite);
void draw_combat_menu(vector2d_t anchor, char* menu_name, const char* menu_options[], int menu_option_count, int selected_index);
void draw_combat_log(vector2d_t anchor, const char* combat_log_message);

#endif//DRAW_COMBAT_MODE_H