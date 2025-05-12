#ifndef STATS_WINDOW_H
#define STATS_WINDOW_H
#include "../../character/character.h"

int init_stats_mode();
void render_stats_window(character_t* player);
void draw_stats_menu(const char* title, const char* options[], int option_count, int selected_index, const char* footer);
void draw_stats_log(const char* message);
void update_stats_local(void);
void shutdown_stats_mode();

#endif//STATS_WINDOW_H
