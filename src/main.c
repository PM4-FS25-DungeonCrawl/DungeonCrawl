#include "game.h"
#include "logging/logger.h"

int main(void) {
    log_msg(INFO, "Main", "init game");
    return init_game();
}
