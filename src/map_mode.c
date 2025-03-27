#include <stdio.h>
#include "map_mode.h"
#include "../include/termbox2.h"
#include "../debug/debug.h"

enum map_tile (*map)[WIDTH][HEIGHT];

int playerX = 1;
int playerY = 1;

void setNewMap(enum map_tile (*newMap)[WIDTH][HEIGHT], int newPlayerX, int newPlayerY) {
    map = newMap;
    playerX = newPlayerX;
    playerY = newPlayerY;
}

void draw_maze(void) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == playerX && y == playerY) {
                tb_printf(x, y, TB_RED, TB_BLACK, "@");
            } else {
                switch ((*map)[x][y]) {
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
                    default:
                        //TODO log error
                        break;
                }
            }
        }
    }
}

void draw_ui(void) {
    tb_printf(0, HEIGHT, TB_WHITE, TB_BLACK, "HP: 100");
}

void handle_input(const struct tb_event *event) {
    int new_x = playerX;
    int new_y = playerY;
    if (event->key == TB_KEY_ARROW_UP) new_y--;
    if (event->key == TB_KEY_ARROW_DOWN) new_y++;
    if (event->key == TB_KEY_ARROW_LEFT) new_x--;
    if (event->key == TB_KEY_ARROW_RIGHT) new_x++;

    if (new_x >= 0 && new_x < WIDTH && new_y >= 0 && new_y < HEIGHT) {
        if ((*map)[new_x][new_y] == FLOOR) {
            //TODO: make a function "move_player" out of this, there you can also handle uncovering more of the map :)
            playerX = new_x;
            playerY = new_y;
        }
    }
}

int mapModeUpdate() {
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
