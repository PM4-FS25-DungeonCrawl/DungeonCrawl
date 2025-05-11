#ifndef DRAW_COMBAT_MODE_H
#define DRAW_COMBAT_MODE_H

#include "../../character/character_fw.h"
#include "../../common.h"
#include "../combat_mode.h"

#define HELMET_WIDTH 29

#define ENEMY_SPRITE_SIZE 30

vector2d_t draw_combat_view(vector2d_t anchor, const character_t* player, const character_t* enemy, enemy_race_t race, bool red_enemy_sprite);
void draw_combat_menu(vector2d_t anchor, const char* menu_name, string_max_t* menu_options, int menu_option_count, int selected_index, const char* tail_msg);
void draw_combat_log(vector2d_t anchor, const char* combat_log_message);
void draw_game_over();
void clear_combat_view(void);

#endif//DRAW_COMBAT_MODE_H
