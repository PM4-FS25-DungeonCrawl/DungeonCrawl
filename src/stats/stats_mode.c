#include "stats_mode.h"

#include "./draw/draw_stats.h"
#include "local/stats_mode_local.h"

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
        const char* menu_options[4];

        menu_options[0] = stats_mode_strings[STRENGTH_STR];
        menu_options[1] = stats_mode_strings[INTELLIGENCE_STR];
        menu_options[2] = stats_mode_strings[DEXTERITY_STR];
        menu_options[3] = stats_mode_strings[CONSTITUTION_STR];

        draw_stats_menu(
                stats_mode_strings[STATS_MENU_TITLE],
                menu_options,
                4,
                selected_index, "");
        // Check for input
        ncinput input;
        // Remove unused variable
        int ret = (int) notcurses_get_nblock(nc, &input);
        if (ret > 0) {
            // Handle arrow keys
            if (input.id == NCKEY_UP) {
                selected_index = (selected_index > 0) ? selected_index - 1 : MAX_STATS - 1;
            } else if (input.id == NCKEY_DOWN) {
                selected_index = (selected_index + 1) % MAX_STATS;
            } else if (input.id == NCKEY_ENTER) {
                if (player->skill_points > 0) {
                    switch (selected_index) {
                        case 0:
                            raise_skill(&player->base_stats, STRENGTH, player->skill_points);
                            break;
                        case 1:
                            raise_skill(&player->base_stats, INTELLIGENCE, player->skill_points);
                            break;
                        case 2:
                            raise_skill(&player->base_stats, DEXTERITY, player->skill_points);
                            break;
                        case 3:
                            raise_skill(&player->base_stats, CONSTITUTION, player->skill_points);
                            break;
                        default:;
                    }
                    player->skill_points--;
                    update_character_resources(&player->max_resources, &player->base_stats);
                } else {
                    char word[MAX_STRING_LENGTH - 4];
                    snprintf(word, MAX_STRING_LENGTH - 4, "%s: 0", stats_mode_strings[AVAILABLE_SKILL_POINTS_STR]);
                    ncplane_putstr_yx(stdplane, 20, 17, word);
                    notcurses_render(nc);
                }
            } else if (input.id == NCKEY_ESC || input.id == 'l' || input.id == 'L') {
                // Clear the screen before drawing a new menu
                ncplane_erase(stdplane);
                points_allocated_or_esc = true;
            }

            // Re-render after input
            notcurses_render(nc);
        }
    }
    ncplane_erase(stdplane);
    notcurses_render(nc);
}
