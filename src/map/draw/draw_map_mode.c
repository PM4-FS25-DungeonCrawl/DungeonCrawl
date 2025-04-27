#include "draw_map_mode.h"

#include "../../../include/termbox2.h"
#include "../../asciiart/ascii.h"
#include "../../common.h"
#include "../../logging/logger.h"

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
    NULL_PTR_HANDLER_RETURN(arr, , "Draw Map Mode", "In draw_map_mode given array is NULL");
    CHECK_ARG_RETURN(height <= 0 || width <= 0, , "Draw Map Mode", "In draw_map_mode given height or width is zero or negative");
    CHECK_ARG_RETURN(anchor.dx < 0 || anchor.dy < 0, , "Draw Map Mode", "In draw_map_mode given anchor is negative");
    CHECK_ARG_RETURN(player_pos.dx < 0 || player_pos.dy < 0 || player_pos.dx >= width || player_pos.dy >= height, , "Draw Map Mode", "In draw_map_mode given player position is negative or out of bounds");

    tb_printf(anchor.dx + width / 2 - 7, anchor.dy, TB_RED, TB_BLACK, "Dungeon Crawl");
    vector2d_t map_anchor = {anchor.dx, anchor.dy + 2};

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            const int draw_x = x + map_anchor.dx;
            const int draw_y = y + map_anchor.dy;

            if (x == player_pos.dx && y == player_pos.dy) {
                tb_printf(draw_x, draw_y, TB_RED, TB_BLACK, "@");
                continue;
            }

            //calculated access index
            const int access_idx = x * height + y;

            switch (arr[access_idx]) {
                case WALL:
                    tb_printf(draw_x, draw_y, TB_BLUE, TB_BLUE, "#");
                    break;
                case FLOOR:
                    tb_printf(draw_x, draw_y, TB_WHITE, TB_BLACK, " ");
                    break;
                case START_DOOR:
                    tb_printf(draw_x, draw_y, TB_GREEN, TB_BLACK, "#");
                    break;
                case EXIT_DOOR:
                    tb_printf(draw_x, draw_y, TB_YELLOW, TB_BLACK, "#");
                    break;
                case KEY:
                    tb_printf(draw_x, draw_y, TB_YELLOW, TB_BLACK, "$");
                    break;
                case GOBLIN:
                    tb_printf(draw_x, draw_y, TB_WHITE, TB_RED, "!");
                    break;
                case HIDDEN:
                    tb_printf(draw_x, draw_y, TB_WHITE, TB_WHITE, " ");
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
    tb_printf(ui_anchor.dx, ui_anchor.dy, TB_WHITE, TB_BLACK, "HP: 100");
    tb_printf(ui_anchor.dx, ui_anchor.dy + 1, TB_WHITE, TB_BLACK, "Press 'M' for Menu");
    tb_printf(ui_anchor.dx, ui_anchor.dy + 2, TB_WHITE, TB_BLACK, "Press 'L' for Stats");
    tb_printf(ui_anchor.dx, ui_anchor.dy + 3, TB_WHITE, TB_BLACK, "Player Position: %d, %d", player_pos.dx, player_pos.dy);

    //draw a ascii art helmet
    for (int i = 0; i < HELMET_HEIGHT; i++) {
        tb_printf(ui_anchor.dx, ui_anchor.dy + i + 3, TB_WHITE, TB_BLACK, "%s", ascii_helmet[i]);
    }
}
