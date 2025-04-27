#ifndef STATS_WINDOW_H
#define STATS_WINDOW_H
#include "../../character/character.h"


void render_stats_window(character_t* player);
void draw_stats_menu(const char* title, const string_max_t options[], int option_count, int selected_index, const char* footer);
void draw_stats_log(const char* message);
#endif//STATS_WINDOW_H
