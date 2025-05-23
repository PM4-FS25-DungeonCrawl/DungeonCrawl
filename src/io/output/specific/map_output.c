#include "map_output.h"

#include "../../../asciiart/ascii.h"
#include "../../../common.h"
#include "../../../logging/logger.h"
#include "../../io_handler.h"
#include "../common/output_handler.h"


void draw_map_mode(const map_tile_t* arr, const int height, const int width, const vector2d_t anchor,
                   const vector2d_t player_pos) {
    NULL_PTR_HANDLER_RETURN(arr, , "Draw Map Mode", "In draw_map_mode given array is NULL");
    CHECK_ARG_RETURN(height <= 0 || width <= 0, , "Draw Map Mode",
                     "In draw_map_mode given height or width is zero or negative");
    CHECK_ARG_RETURN(anchor.dx < 0 || anchor.dy < 0, , "Draw Map Mode", "In draw_map_mode given anchor is negative");
    CHECK_ARG_RETURN(player_pos.dx < 0 || player_pos.dy < 0 || player_pos.dx >= width || player_pos.dy >= height, ,
                     "Draw Map Mode", "In draw_map_mode given player position is negative or out of bounds");

    // Print the title using centralized IO handler
    print_text(anchor.dy, anchor.dx + width / 2 - 7, "Dungeon Crawl", RED_TEXT_COLORS);

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
    print_text_default(y++, x, "Press 'M' for Menu");
    print_text_default(y++, x, "Press 'L' for Stats");
    print_text_default(y++, x, "Press 'I' for Inventory");

    // Format player position string
    char pos_str[64];
    snprintf(pos_str, sizeof(pos_str), "Player Position: %d, %d", player_pos.dx, player_pos.dy);
    print_text_default(y + 4, x, pos_str);
}