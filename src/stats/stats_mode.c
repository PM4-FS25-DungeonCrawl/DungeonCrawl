#include "stats_mode.h"

#include "../combat/ability.h"
#include "../io/input/input_handler.h"
#include "../io/io_handler.h"
#include "../io/output/common/common_output.h"
#include "../io/output/specific/stats_output.h"
#include "../local/local.h"
#include "../local/local_strings.h"

// Change from definition to declaration
extern struct notcurses* nc;
int selected_index = 0;

stats_result_t stats_mode(character_t* player) {
    clear_screen();
    stats_result_t result = STATS_WINDOW;

    // Draw stats window
    render_stats_window(player);
    const char* menu_options[4];

    menu_options[0] = &local_strings[stmo_ability_strength.idx].characters[0];
    menu_options[1] = &local_strings[stmo_ability_intelligence.idx].characters[0];
    menu_options[2] = &local_strings[stmo_ability_dexterity.idx].characters[0];
    menu_options[3] = &local_strings[stmo_ability_constitution.idx].characters[0];

    draw_stats_menu(
            local_strings[stmo_menu_stats_title.idx].characters,
            menu_options,
            4,
            selected_index, "");
    // Check for input
    input_event_t input_event;
    if (get_input_nonblocking(&input_event)) {
        // Handle input using logical input types
        switch (input_event.type) {
            case INPUT_UP:
                selected_index = (selected_index > 0) ? selected_index - 1 : MAX_STATS - 1;
                break;
            case INPUT_DOWN:
                selected_index = (selected_index + 1) % MAX_STATS;
                break;
            case INPUT_CONFIRM:
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
                }
                break;
            case INPUT_CANCEL:
            case INPUT_STATS:
                // Clear the screen before drawing a new menu
                clear_screen();
                result = STATS_EXIT;
                break;
            default:
                break;
        }
    }

    render_io_frame();
    return result;
}
