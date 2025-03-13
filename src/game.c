#include <stdio.h>
#include <time.h>
#include "game.h"
#include "../include/termbox2.h"
#include "../debug/debug.h"



int map[HEIGHT][WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,0,1},
    {1,0,1,0,0,0,1,0,0,0,0,0,0,1,1,0,0,1,0,1},
    {1,0,1,0,1,0,1,1,1,1,1,1,0,1,1,1,0,1,0,1},
    {1,0,1,0,1,0,0,0,0,0,0,1,0,0,0,1,0,1,0,1},
    {1,0,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int player_x = 1, player_y = 1;
int ui_char_x = 0;

void draw_maze(void) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (map[y][x] == 1) {
                tb_printf(x, y,TB_BLUE,TB_BLUE,"#");
            } else if (x == player_x && y == player_y) {
                tb_printf(x, y, TB_RED, TB_BLACK, "@");
            } else {
                tb_printf(x, y, TB_BLACK, TB_BLACK, " ");
            }
        }
    }
}

void draw_ui(void) {
    tb_printf(ui_char_x, HEIGHT + 1, TB_BLACK, TB_BLACK, " ");
    tb_printf(0, HEIGHT, TB_WHITE, TB_BLACK, "HP: 100");
    tb_printf(ui_char_x, HEIGHT + 1, TB_YELLOW, TB_BLACK, "*");
}

void handle_input(struct tb_event *event) {
    int new_x = player_x, new_y = player_y;
    if (event->key == TB_KEY_ARROW_UP)    new_y--;
    if (event->key == TB_KEY_ARROW_DOWN)  new_y++;
    if (event->key == TB_KEY_ARROW_LEFT)  new_x--;
    if (event->key == TB_KEY_ARROW_RIGHT) new_x++;

    if (map[new_y][new_x] == 0) {
        player_x = new_x;
        player_y = new_y;
    }
}

long long current_time_millis(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

int add(int a, int b) {
    return a + b;
}

int init_game(){
    if (tb_init()!= 0) {
        fprintf(stderr, "Failed to initialize termbox \n");
        return 1;
    }
    tb_set_output_mode(TB_OUTPUT_NORMAL);

    long long last_update = current_time_millis();
    long long current_time = last_update;
    struct tb_event ev;
    int ret = 0;

    while (1) {
        ret = tb_peek_event(&ev,10);
        db_printEventStruct(3, 20, &ev);

        current_time = current_time_millis();
        if (current_time - last_update >= 200) {
            tb_printf(ui_char_x, HEIGHT + 1, TB_BLACK, TB_BLACK, " ");
            ui_char_x++;
            if (ui_char_x >= WIDTH) ui_char_x = 1;
            last_update = current_time;
        }

        if (ret == TB_OK) {
            tb_printf(50, 50, TB_WHITE, TB_BLACK, "%d",ev);
            if (ev.key == TB_KEY_ESC || ev.key == TB_KEY_CTRL_C) break;
            handle_input(&ev);
        }
        draw_maze();
        draw_ui();
        tb_present();
    }

    tb_shutdown();
    return 0;
}
