#ifndef INPUT_H
#define INPUT_H

#include <notcurses/notcurses.h>
#include "game.h"

bool handle_input(struct notcurses* nc,struct ncinput* ni, enum game_state current_state);

#endif // !INPUT_H


