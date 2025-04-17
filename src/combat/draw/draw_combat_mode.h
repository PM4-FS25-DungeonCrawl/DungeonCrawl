#ifndef DRAW_COMBAT_MODE_H
#define DRAW_COMBAT_MODE_H

#include "../../character/character_fw.h"
#include "../../common.h"

vector2d_t draw_combat_view(vector2d_t anchor, const character_t* player, const character_t* enemy, const char* enemy_sprite, int sprite_height, bool red_enemy_sprite);
void draw_combat_menu(vector2d_t anchor, const char* menu_name, const char* menu_options[], int menu_option_count, int selected_index, const char* tail_msg);
void draw_combat_log(vector2d_t anchor, const char* combat_log_message);

#endif//DRAW_COMBAT_MODE_H
