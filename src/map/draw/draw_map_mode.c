#include "draw_map_mode.h"

#include "../../../include/termbox2.h"
#include "../../asciiart/ascii.h"
#include "../../logging/logger.h"


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
void draw_map_mode(const map_tile_t* arr, const int height, const int width, const vector2d_t anchor, const vector2d_t player_pos) {
    if (arr == NULL) {
        log_msg(ERROR, "Draw Map Mode", "In draw_map_mode given array is NULL");
        return;
    }
    if (height <= 0 || width <= 0) {
        log_msg(ERROR, "Draw Map Mode", "In draw_map_mode given height or width is zero or negative");
        return;
    }
    if (anchor.dx < 0 || anchor.dy < 0) {
        log_msg(ERROR, "Draw Map Mode", "In draw_map_mode given anchor is negative");
        return;
    }
    if (player_pos.dx < anchor.dx || player_pos.dy < anchor.dy || player_pos.dx >= anchor.dx + width || player_pos.dy >= anchor.dy + height) {
        log_msg(ERROR, "Draw Map Mode", "In draw_map_mode given player position out of bounds");
        return;
    }

    for (int y = anchor.dx; y < height + anchor.dx; y++) {
        for (int x = anchor.dy; x < width + anchor.dy; x++) {
            if (x == player_pos.dx && y == player_pos.dy) {
                tb_printf(x, y, TB_RED, TB_BLACK, "@");
                continue;
            }

            //calculated access index
            const int access_idx = x * height + y;

            switch (arr[access_idx]) {
                case WALL:
                    tb_printf(x, y, TB_BLUE, TB_BLUE, "#");
                    break;
                case FLOOR:
                    tb_printf(x, y, TB_WHITE, TB_BLACK, " ");
                    break;
                case START_DOOR:
                    tb_printf(x, y, TB_GREEN, TB_BLACK, "#");
                    break;
                case EXIT_DOOR:
                    tb_printf(x, y, TB_YELLOW, TB_BLACK, "#");
                    break;
                case KEY:
                    tb_printf(x, y, TB_YELLOW, TB_BLACK, "$");
                    break;
                case GOBLIN:
                    tb_printf(x, y, TB_WHITE, TB_RED, "!");
                    break;
                case HIDDEN:
                    tb_printf(x, y, TB_WHITE, TB_WHITE, " ");
                    break;
                default:
                    log_msg(ERROR, "map_mode", "Unknown tile type: %d", arr[access_idx]);
                    return;
            }
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
    tb_printf(x, y, TB_WHITE, TB_BLACK, "HP: 100");
    tb_printf(x, y + 1, TB_WHITE, TB_BLACK, "Player Position: %d, %d", player_pos.dx, player_pos.dy);

    //draw a ascii art helmet
    for (int i = y + 3; i < y + 3 + HELMET_HEIGHT; i++) {
        tb_printf(x, i, TB_WHITE, TB_BLACK, "%s", ascii_helmet[i - y - 3]);
    }
}
