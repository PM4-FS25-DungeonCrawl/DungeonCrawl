#include <stdio.h>
#include <stdbool.h>
#include "game.h"
#include "map/map_mode.h"
#include "map/map_generator.h"
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

    init_map_mode();

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
                if (map_mode_update()) {
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
