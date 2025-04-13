#include "src/game.h"
#include "src/map/map_mode.h"
#include <notcurses/notcurses.h>

// enum game_state { MAIN_MENU, MAP_MODE, COMBAT_MODE, GENERATE_MAP, EXIT };

bool handle_input(struct notcurses* nc, struct ncinput* ni, enum game_state current_state) {
    switch (current_state) {
        case MAP_MODE:
            if (ni->id == 'w') move_player(DIR_UP);
            else if (ni->id == 's') move_player(DIR_DOWN);
            else if (ni->id == 'a') move_player(DIR_LEFT);
            else if (ni->id == 'd') move_player(DIR_RIGHT);
            else if (ni->id == 'q') return false; // Quit
            break;

        default:
            break;
    }

    return true; // continue running
}
