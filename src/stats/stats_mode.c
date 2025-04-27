#include "stats_mode.h"
#include "./draw/draw_stats.h"
#include "../../include/termbox2.h"

void stats_mode(character_t* player) {
    int selected_index = 0;

    bool points_allocated_or_esc = false;

    while (!points_allocated_or_esc) {
        // Draw stats menu

        render_stats_window(player);
        draw_stats_menu(
            "Stats Menu",
            (string_max_t[]) {
                {"Strength"},
                {"Intelligence"},
                {"Dexterity"},
                {"Constitution"}
            },
            4,
            selected_index,
            "Use arrow keys to navigate, Enter to allocate, ESC to exit."
        );

        // Check for input
        struct tb_event event;
        const int ret = tb_peek_event(&event, 10);

        if (ret == TB_OK) {
            if (event.key == TB_KEY_ARROW_UP) {
                selected_index = (selected_index - 1 + 4) % 4;
            } else if (event.key == TB_KEY_ARROW_DOWN) {
                selected_index = (selected_index + 1) % 4;
            } else if (event.key == TB_KEY_ENTER) {
                if (player->skill_points >0) {
                    switch (selected_index) {
                        case 0: raise_skill(&player->base_stats, 0, 1,player->skill_points); break;
                        case 1: raise_skill(&player->base_stats, 1, 1,player->skill_points); break;
                        case 2: raise_skill(&player->base_stats, 2, 1,player->skill_points); break;
                        case 3: raise_skill(&player->base_stats, 3, 1,player->skill_points); break;
                        default: ;
                    }
                    player->skill_points--;
                }else {
                    tb_print(20, 17, TB_RED, TB_DEFAULT, "Not enough skill points!");
                    tb_present();
                }
            } else if (event.key == TB_KEY_ESC) {
                tb_clear();
                points_allocated_or_esc = true;

            }
        }
    }
}