#ifndef DRAW_COMBAT_MODE_H
#define DRAW_COMBAT_MODE_H

#include "../../character/character_fw.h"
#include "../../common.h"

vector2d_t draw_inventory_view(const vector2d_t anchor, const character_t* player);
void draw_inventory_menu(const vector2d_t anchor, const char* menu_name, const char* header_msg, char** menu_options, const int menu_option_count, const int selected_index, const char* key_msg, const char* tail_msg);
void draw_inventory_log(vector2d_t anchor, const char* inventory_log_message);

#endif//DRAW_COMBAT_MODE_H
