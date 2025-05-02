#include "stats_mode.h"
#include "./draw/draw_stats.h"
#include "src/local/local_strings.h"
#include "src/local/local.h"

// Change from definition to declaration
extern struct notcurses* nc;
extern struct ncplane* stdplane;

void stats_mode(character_t* player) {
    stdplane = notcurses_stdplane(nc);
    int selected_index = 0;

    bool points_allocated_or_esc = false;

    while (!points_allocated_or_esc) {
        // Draw stats menu

        render_stats_window(player);
        draw_stats_menu(
            "Stats Menu",
            (string_max_t[]) {
                {local_strings[stmo_ability_strength.idx].characters,},
                {local_strings[stmo_ability_intelligence.idx].characters,},
                {local_strings[stmo_ability_dexterity.idx].characters,},
                {local_strings[stmo_ability_constitution.idx].characters,}
            },
            4,
            selected_index,""
        );

        // Check for input
        ncinput input;
        // Remove unused variable
        int ret = notcurses_get_nblock(nc, &input);
        if (ret > 0) {
            // Handle arrow keys
            if (input.id == NCKEY_UP) {
                selected_index = (selected_index - 1 + 4) % 4;
            } else if (input.id == NCKEY_DOWN) {
                selected_index = (selected_index + 1) % 4;
            } else if (input.id == NCKEY_ENTER) {
                if (player->skill_points > 0) {
                    switch (selected_index) {
                        case 0: raise_skill(&player->base_stats, 0, 1, player->skill_points); break;
                        case 1: raise_skill(&player->base_stats, 1, 1, player->skill_points); break;
                        case 2: raise_skill(&player->base_stats, 2, 1, player->skill_points); break;
                        case 3: raise_skill(&player->base_stats, 3, 1, player->skill_points); break;
                        default: ;
                    }
                    player->skill_points--;
                    update_character_resources(&player->max_resources, &player->base_stats);
                } else {
                    char word[MAX_STRING_LENGTH];
                    snprintf(word, MAX_STRING_LENGTH, "%s: 0", local_strings[stmo_option_skillpoints.idx].characters);
                    ncplane_putstr_yx(stdplane, 20, 17, word);
                    notcurses_render(nc);
                }
            } else if (input.id == NCKEY_ESC) {
                // Clear the screen before drawing a new menu
                ncplane_erase(stdplane);
                points_allocated_or_esc = true;
            }

            // Re-render after input
            notcurses_render(nc);
        }
    }
}