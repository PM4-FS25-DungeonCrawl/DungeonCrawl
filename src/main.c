#include "game.h"
#include "logging/logger.h"

int main(void) {
    log_msg(INFO, "Main", "game starting");
    return init_game();
}
