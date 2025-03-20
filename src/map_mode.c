#include <stdio.h>
#include "map_mode.h"
#include "../include/termbox2.h"
#include "../debug/debug.h"


int map[HEIGHT][WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

int player_x = 1;
int player_y = 1;

void draw_maze(void) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (map[y][x] == 1) {
                tb_printf(x, y,TB_BLUE,TB_BLUE, "#");
            } else if (x == player_x && y == player_y) {
                tb_printf(x, y, TB_RED, TB_BLACK, "@");
            } else {
                tb_printf(x, y, TB_BLACK, TB_BLACK, " ");
            }
        }
    }
}

void draw_ui(void) {
    tb_printf(0, HEIGHT, TB_WHITE, TB_BLACK, "HP: 100");
}

void handle_input(const struct tb_event *event) {
    int new_x = player_x;
    int new_y = player_y;
    if (event->key == TB_KEY_ARROW_UP) new_y--;
    if (event->key == TB_KEY_ARROW_DOWN) new_y++;
    if (event->key == TB_KEY_ARROW_LEFT) new_x--;
    if (event->key == TB_KEY_ARROW_RIGHT) new_x++;

    if (map[new_y][new_x] == 0) {
        player_x = new_x;
        player_y = new_y;
    }
}

int mapModeStep() {
    struct tb_event ev;
    int ret = tb_peek_event(&ev, 10);
    db_printEventStruct(3, 20, &ev);

    if (ret == TB_OK) {
        tb_printf(50, 50, TB_WHITE, TB_BLACK, "%d", ev.type);
        if (ev.key == TB_KEY_ESC || ev.key == TB_KEY_CTRL_C) return 1;
        handle_input(&ev);
    }
    draw_maze();
    draw_ui();
    tb_present();

    return 0;
}
