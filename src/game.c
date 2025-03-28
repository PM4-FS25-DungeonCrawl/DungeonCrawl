#include <stdio.h>
#include <stdbool.h>
#include "game.h"
#include "map_mode.h"
#include "map_generator.h"
#include "../include/termbox2.h"

enum game_state {
    MAIN_MENU,
    MAP_MODE,
    COMBAT_MODE,
    GENERATE_MAP,
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

    bool doRun = true;
    enum game_state currentState = GENERATE_MAP;

    while (doRun) {
        switch (currentState) {
            case MAIN_MENU:
                break;
            case GENERATE_MAP:
                generate_map();
                currentState = MAP_MODE;
                break;
            case MAP_MODE:
                if (mapModeUpdate()) {
                    currentState = EXIT;
                }
                break;
            case COMBAT_MODE:
                break;
            case EXIT:
                doRun = false;
            default:
        }
    }

    tb_shutdown();
    return 0;
}
