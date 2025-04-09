#include "game.h"
#include "logging/logger.h"
#include "local/local.h"

int main(void) {
    init_logger();
    init_local();
    log_msg(INFO, "Main", "init game");
    return init_game();
}
