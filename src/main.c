#include "game.h"
#include "logging/logger.h"

int main(void) {
    init_logger();
    log_msg(INFO, "Main", "init game");
    return init_game();
}
