#include "game.h"
#include "local/local.h"
#include "logging/logger.h"

int main(void) {
    init_logger();
    init_local();
    log_msg(INFO, "Main", "init game");
    int result = init_game();
    log_msg(INFO, "Main", "game finished with exit code %d", result);
    return result;
}