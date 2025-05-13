#include "map_output.h"

#include "../../../asciiart/ascii.h"
#include "../../../common.h"
#include "../../../logging/logger.h"
#include "../../io_handler.h"
#include "../common/common_output.h"

/**
 * @brief Draws the map mode UI based on the given parameters.
 *
 * @param arr The map array to be drawn
 * @param height The height of the map
 * @param width The width of the map
 * @param anchor The anchor position of the map mode, defined as the top left corner
 * @param player_pos The position of the player
 *
 * @note This function checks makes different checks to ensure the given parameters are valid.
 * The checks are done in the following order:
 * - check if the array is NULL
 * - check if the height and width are greater than 0
 * - check if the anchor position is greater or equal 0
 * - check if the player position is within the bounds of the map
 * If any of the checks fail, an error message is logged and the function returns.
 */
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
    render_io_frame();
}

/**
 * @brief Draws the player information of the map mode.
 *
 * @param x The x position of the player info to be drawn
 * @param y The y position of the player info to be drawn
 * @param player_pos The current player position
 */
void draw_player_info(int x, int y, const vector2d_t player_pos) {
    // Player information using centralized IO
    print_text_default(y++, x, "Press 'M' for Menu");
    print_text_default(y++, x, "Press 'L' for Stats");
    print_text_default(y++, x, "Press 'I' for Inventory");

    // Format player position string
    char pos_str[64];
    snprintf(pos_str, sizeof(pos_str), "Player Position: %d, %d", player_pos.dx, player_pos.dy);
    print_text_default(y + 4, x, pos_str);

    // Draw ASCII helmet
    const char* helmet_lines[HELMET_HEIGHT];
    for (int i = 0; i < HELMET_HEIGHT; i++) {
        helmet_lines[i] = ascii_helmet[i];
    }

    // Use the multi-line string printing function for the helmet
    //print_text_multi_strings_default(y++, x, helmet_lines, HELMET_HEIGHT);
}