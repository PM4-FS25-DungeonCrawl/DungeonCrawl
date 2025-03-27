#include <stdio.h>
#include <stdbool.h>
#include "game.h"
#include "map_mode.h"
#include "../include/termbox2.h"
#include "logging/logger.h"

enum game_state {
    MAIN_MENU,
    MAP_MODE,
    COMBAT_MODE,
    EXIT
};

int add(int a, int b) {
    return a + b;
}

int init_game(){
    if (tb_init()!= 0) {
        fprintf(stderr, "Failed to initialize termbox \n");
        return 1;
    }
    tb_set_output_mode(TB_OUTPUT_NORMAL);

    init_map_mode();

    log_msg(INFO, "Game", "game loop starting");
    bool doRun = true;
    enum game_state currentState = MAP_MODE;

    while (doRun) {
        switch (currentState) {
            case MAIN_MENU:
                break;
            case MAP_MODE:
                if (map_mode_update()) {
                    currentState = EXIT;
                }
                break;
            case COMBAT_MODE:
                break;
            case EXIT:
                close_log_file(1);
                doRun = false;
            default:
        }
    }

    tb_shutdown();
    return 0;
}
