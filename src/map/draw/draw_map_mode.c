#include "draw_map_mode.h"

#include "../../asciiart/ascii.h"
#include "../../logging/logger.h"
#include "src/common.h"

#include <notcurses/notcurses.h>

// External declaration of the global notcurses instance and standard plane
extern struct notcurses* nc;
extern struct ncplane* stdplane;

void draw_player_info(int x, int y, vector2d_t player_pos);

/**
 * @brief Draws the map mode ui based on the given parameters.
 *
 * @param arr the map array to be drawn
 * @param height the height of the map
 * @param width the width of the map
 * @param anchor the anchor position of the map mode, defined as the top left corner
 * @param player_pos the position of the player
 *
 * @note this function checks makes different checks to ensure the given parameters are valid.
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

    ncplane_set_channels(stdplane, RED_ON_BLACK);
    ncplane_printf_yx(stdplane, anchor.dy, anchor.dx + width / 2 - 7, "Dungeon Crawl");

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Setup channels (colors)
            uint64_t channels = 0;
            const char* ch;

            if (x == player_pos.dx && y == player_pos.dy) {
                // Player character
                channels = NCCHANNELS_INITIALIZER(255, 255, 255, 0, 0, 0);// White on black
                ncchannels_set_fg_rgb8(&channels, 255, 0, 0);             // Red foreground
                ncplane_set_channels(stdplane, channels);
                ncplane_putchar_yx(stdplane, y + anchor.dy, x + anchor.dx, '@');
                continue;
            }

            //calculated access index
            const int access_idx = x * height + y;

            switch (arr[access_idx]) {
                case WALL:
                    channels = NCCHANNELS_INITIALIZER(0x80, 0x40, 0x20, 0x80, 0x40, 0x20);// Brown on brown
                    ch = "#";
                    break;
                case FLOOR:
                    channels = NCCHANNELS_INITIALIZER(0xff, 0xff, 0xff, 0, 0, 0);// White on black
                    ch = " ";
                    break;
                case START_DOOR:
                    channels = NCCHANNELS_INITIALIZER(0, 0xff, 0, 0, 0, 0);// Green on black
                    ch = "#";
                    break;
                case EXIT_DOOR:
                    channels = NCCHANNELS_INITIALIZER(0xff, 0xff, 0, 0, 0, 0);// Yellow on black
                    ch = "#";
                    break;
                case KEY:
                    channels = NCCHANNELS_INITIALIZER(0xff, 0xff, 0, 0, 0, 0);// Yellow on black
                    ch = "$";
                    break;
                case LIFE_FOUNTAIN:
                    channels = NCCHANNELS_INITIALIZER(0xff, 0xff, 0xff, 0, 0xff, 0);// Black on green
                    ch = "+";
                    break;
                case MANA_FOUNTAIN:
                    channels = NCCHANNELS_INITIALIZER(0xff, 0xff, 0xff, 0, 0, 0xff);// Black on blue
                    ch = "+";
                    break;
                case GOBLIN:
                    channels = NCCHANNELS_INITIALIZER(0xff, 0xff, 0xff, 0xff, 0, 0);// White on red
                    ch = "!";
                    break;
                case HIDDEN:
                    channels = NCCHANNELS_INITIALIZER(0xff, 0xff, 0xff, 0xff, 0xff, 0xff);// White on white
                    ch = " ";
                    break;
                default:
                    log_msg(ERROR, "map_mode", "Unknown tile type: %d", arr[access_idx]);
                    ch = " ";
            }

            ncplane_set_channels(stdplane, channels);
            ncplane_putstr_yx(stdplane, y + anchor.dy, x + anchor.dx, ch);
        }
    }

    draw_player_info(anchor.dx, anchor.dy + height + 1, player_pos);
}

/**
 * @brief Draws the player information of the map mode.
 *
 * @param x the x position of the player info to be drawn
 * @param y the y position of the player info to be drawn
 * @param player_pos the current player position
 */
void draw_player_info(const int x, const int y, const vector2d_t player_pos) {
    uint64_t channels = NCCHANNELS_INITIALIZER(0xff, 0xff, 0xff, 0, 0, 0);// White on black
    ncplane_set_channels(stdplane, channels);

    ncplane_printf_yx(stdplane, y, x, "HP: 100");
    ncplane_printf_yx(stdplane, y + 1, x, "Press 'M' for Menu");
    ncplane_printf_yx(stdplane, y + 2, x, "Player Position: %d, %d", player_pos.dx, player_pos.dy);

    //draw a ascii art helmet
    for (int i = y + 3; i < y + 3 + HELMET_HEIGHT; i++) {
        ncplane_printf_yx(stdplane, i, x, "%s", ascii_helmet[i - y - 3]);
    }
}
