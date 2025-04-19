#include "draw_map_mode.h"

#include "../../asciiart/ascii.h"
#include "../../common.h"
#include "../../logging/logger.h"

#include <notcurses/notcurses.h>

// External reference to notcurses context
extern struct notcurses* nc;
extern struct ncplane* stdplane;

void draw_player_info(vector2d_t ui_anchor, vector2d_t player_pos);

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
void draw_map_mode(const map_tile_t* arr, const int height, const int width, const vector2d_t anchor, const vector2d_t player_pos) {
    NULL_PTR_HANDLER_VOID(arr, "Draw Map Mode", "In draw_map_mode given array is NULL");
    CHECK_ARG_VOID(height <= 0 || width <= 0, "Draw Map Mode", "In draw_map_mode given height or width is zero or negative");
    CHECK_ARG_VOID(anchor.dx < 0 || anchor.dy < 0, "Draw Map Mode", "In draw_map_mode given anchor is negative");
    CHECK_ARG_VOID(player_pos.dx < 0 || player_pos.dy < 0 || player_pos.dx >= width || player_pos.dy >= height, "Draw Map Mode", "In draw_map_mode given player position is negative or out of bounds");

    // Define color channels
    uint64_t red_on_black = NCCHANNELS_INITIALIZER(255, 0, 0, 0, 0, 0);
    uint64_t blue_on_blue = NCCHANNELS_INITIALIZER(0, 0, 255, 0, 0, 255);
    uint64_t white_on_black = NCCHANNELS_INITIALIZER(255, 255, 255, 0, 0, 0);
    uint64_t green_on_black = NCCHANNELS_INITIALIZER(0, 255, 0, 0, 0, 0);
    uint64_t yellow_on_black = NCCHANNELS_INITIALIZER(255, 255, 0, 0, 0, 0);
    uint64_t white_on_red = NCCHANNELS_INITIALIZER(255, 255, 255, 255, 0, 0);
    uint64_t white_on_white = NCCHANNELS_INITIALIZER(255, 255, 255, 255, 255, 255);

    // Draw title
    ncplane_printf_yx(stdplane, anchor.dy, anchor.dx + width / 2 - 7, "Dungeon Crawl");
    ncplane_set_channels(stdplane, red_on_black);

    vector2d_t map_anchor = {anchor.dx, anchor.dy + 2};

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            const int draw_x = x + map_anchor.dx;
            const int draw_y = y + map_anchor.dy;

            if (x == player_pos.dx && y == player_pos.dy) {
                ncplane_set_channels(stdplane, red_on_black);
                ncplane_putstr_yx(stdplane, draw_y, draw_x, "@");
                continue;
            }

            //calculated access index
            const int access_idx = x * height + y;

            switch (arr[access_idx]) {
                case WALL:
                    ncplane_set_channels(stdplane, blue_on_blue);
                    ncplane_putstr_yx(stdplane, draw_y, draw_x, "#");
                    break;
                case FLOOR:
                    ncplane_set_channels(stdplane, white_on_black);
                    ncplane_putstr_yx(stdplane, draw_y, draw_x, " ");
                    break;
                case START_DOOR:
                    ncplane_set_channels(stdplane, green_on_black);
                    ncplane_putstr_yx(stdplane, draw_y, draw_x, "#");
                    break;
                case EXIT_DOOR:
                    ncplane_set_channels(stdplane, yellow_on_black);
                    ncplane_putstr_yx(stdplane, draw_y, draw_x, "#");
                    break;
                case KEY:
                    ncplane_set_channels(stdplane, yellow_on_black);
                    ncplane_putstr_yx(stdplane, draw_y, draw_x, "$");
                    break;
                case GOBLIN:
                    ncplane_set_channels(stdplane, white_on_red);
                    ncplane_putstr_yx(stdplane, draw_y, draw_x, "!");
                    break;
                case HIDDEN:
                    ncplane_set_channels(stdplane, white_on_white);
                    ncplane_putstr_yx(stdplane, draw_y, draw_x, " ");
                    break;
                default:
                    log_msg(ERROR, "map_mode", "Unknown tile type: %d", arr[access_idx]);
                    return;
            }
        }
    }

    map_anchor.dy += height + 1;

    draw_player_info(map_anchor, player_pos);
}

/**
 * @brief Draws the player information of the map mode.
 * @param ui_anchor The anchor position of the ui, defined as the top left corner
 * @param player_pos The current player position
 */
void draw_player_info(const vector2d_t ui_anchor, const vector2d_t player_pos) {
    uint64_t white_on_black = NCCHANNELS_INITIALIZER(255, 255, 255, 0, 0, 0);
    ncplane_set_channels(stdplane, white_on_black);

    ncplane_printf_yx(stdplane, ui_anchor.dy, ui_anchor.dx, "HP: 100");
    ncplane_printf_yx(stdplane, ui_anchor.dy + 1, ui_anchor.dx, "Press 'M' for Menu");
    ncplane_printf_yx(stdplane, ui_anchor.dy + 2, ui_anchor.dx, "Player Position: %d, %d", player_pos.dx, player_pos.dy);

    //draw a ascii art helmet
    for (int i = 0; i < HELMET_HEIGHT; i++) {
        ncplane_printf_yx(stdplane, ui_anchor.dy + i + 3, ui_anchor.dx, "%s", ascii_helmet[i]);
    }

    // Render the updated screen
    notcurses_render(nc);
}
