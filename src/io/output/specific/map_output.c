/**
 * @file map_output.c
 * @brief Implements outputting to the screen for the map mode.
 */
#include "map_output.h"

#include "../../../common.h"
#include "../../../local/local_handler.h"
#include "../../../logging/logger.h"
#include "../../io_handler.h"
#include "../common/output_handler.h"
#include "../common/text_output.h"
#include "../media/media_files.h"
#include "../media/media_output.h"
#include "../src/map/local/map_mode_local.h"
#include "../src/map/map_mode.h"


void draw_map_mode(const map_tile_t* arr, const int height, const int width, const vector2d_t anchor,
                   const vector2d_t player_pos) {
    NULL_PTR_HANDLER_RETURN(arr, , "Draw Map Mode", "In draw_map_mode given array is NULL");
    CHECK_ARG_RETURN(height <= 0 || width <= 0, , "Draw Map Mode",
                     "In draw_map_mode given height or width is zero or negative");
    CHECK_ARG_RETURN(anchor.dx < 0 || anchor.dy < 0, , "Draw Map Mode", "In draw_map_mode given anchor is negative");
    CHECK_ARG_RETURN(player_pos.dx < 0 || player_pos.dy < 0 || player_pos.dx >= width || player_pos.dy >= height, ,
                     "Draw Map Mode", "In draw_map_mode given player position is negative or out of bounds");

    // Print the title using centralized IO handler
    char* map_title = get_local_string("MAP.TITLE");
    print_text(anchor.dy, anchor.dx + width / 2 - 7, map_title, RED_TEXT_COLORS);
    free(map_title);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Setup channels (colors)
            uint64_t channels = 0;
            const char* ch;

            if (x == player_pos.dx && y == player_pos.dy) {
                // Player character - using centralized print_text
                print_text(y + anchor.dy, x + anchor.dx, "@", RED_TEXT_COLORS);
                continue;
            }

            //calculated access index
            const int access_idx = x * height + y;

            switch (arr[access_idx]) {
                case WALL:
                    channels = WALL_COLORS;
                    ch = "#";
                    break;
                case FLOOR:
                    channels = FLOOR_COLORS;
                    ch = " ";
                    break;
                case START_DOOR:
                    channels = START_DOOR_COLORS;
                    ch = "#";
                    break;
                case EXIT_DOOR:
                    channels = EXIT_DOOR_COLORS;
                    ch = "#";
                    break;
                case KEY:
                    channels = KEY_COLORS;
                    ch = "$";
                    break;
                case LIFE_FOUNTAIN:
                    channels = LIFE_FOUNTAIN_COLORS;
                    ch = "+";
                    break;
                case MANA_FOUNTAIN:
                    channels = MANA_FOUNTAIN_COLORS;
                    ch = "+";
                    break;
                case GOBLIN:
                    channels = GOBLIN_COLORS;
                    ch = "!";
                    break;
                case HIDDEN:
                    channels = HIDDEN_COLORS;
                    ch = " ";
                    break;
                default:
                    log_msg(ERROR, "map_mode", "Unknown tile type: %d", arr[access_idx]);
                    ch = " ";
                    channels = DEFAULT_COLORS;
            }

            // Use centralized IO for printing
            print_text(y + anchor.dy, x + anchor.dx, ch, channels);
        }
    }

    // Draw player info
    draw_player_info(anchor.dx, anchor.dy + height + 1, player_pos);

    // Render the frame using centralized IO
    render_frame();
}


void draw_player_info(int x, int y, const vector2d_t player_pos) {
    // Player information using centralized IO
    print_text_default(y++, x, map_mode_strings[PRESS_KEY_MENU]);
    print_text_default(y++, x, map_mode_strings[PRESS_KEY_STATS]);
    print_text_default(y++, x, map_mode_strings[PRESS_KEY_INVENTORY]);

    // Format current floor information
    char floor_str[64];
    snprintf(floor_str, sizeof(floor_str), "Current Floor: %d", current_floor);
    print_text_default(y++, x, floor_str);

    // Format player position string
    char pos_str[64];
    snprintf(pos_str, sizeof(pos_str), "%s: %d, %d", map_mode_strings[PLAYER_POSITION_STR], player_pos.dx, player_pos.dy);
    print_text_default(y + 2, x, pos_str);
}

void draw_transition_screen(void) {
    // Get screen dimensions
    int width, height;
    get_screen_dimensions(&width, &height);

    // Clear the screen
    clear_screen();

    char* transition_text = get_local_string("MAP.FLOOR.TRANSITION");
    char welcome_msg[256];
    snprintf(welcome_msg, sizeof(welcome_msg), "%s", transition_text);
    free(transition_text);

    int msg_len = strlen(welcome_msg);
    int msg_x = (width - msg_len) / 2;
    int msg_y = height / 4;

    print_text_default(msg_y, msg_x, welcome_msg);

    // Render the frame using centralized IO handler
    render_frame();

    // Wait for user input while animation plays
    input_event_t input_event;
    display_gif_at_interruptible(PLAYER_RUN_GIF, (width - PLAYER_RUN_WIDTH) / 2, msg_y + 1, PLAYER_RUN_HEIGHT, PLAYER_RUN_WIDTH, SCALE_STRETCH, 5, true, &input_event);

    // If no input was captured during animation, wait for input
    if (input_event.type == INPUT_NONE) {
        get_input_blocking(&input_event);
    }

    // Clear the screen after input
    media_cleanup();
}
