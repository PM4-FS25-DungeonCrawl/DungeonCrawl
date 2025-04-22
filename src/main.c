#include <time.h>

#include "game.h"
#include "combat/combat_mode.h"
#include "local/local.h"
#include "logging/logger.h"
#include "map/map_mode.h"
#include "menu/main_menu.h"
#include "game_data.h"

int init() {
    // seeding random function
    srand(time(NULL));

    init_logger();
    if (init_local() != 0) {
        log_msg(ERROR, "Main", "Failed to initialize local");
        return FAIL_LOCAL_INIT;
    }

    // Initialize database connection
    if (db_open(&db_connection, "resources/database/game/dungeoncrawl_game.db") != DB_OPEN_STATUS_SUCCESS) {
        log_msg(ERROR, "Game", "Failed to open database");
        return 1;
    }
    create_tables_game_state(&db_connection); // only for dungeoncrawl_game.db

    init_map_mode();
    init_main_menu();
    init_combat_mode();
    if (init_game_data()) {
        log_msg(ERROR, "Game", "Failed to initialize game components");
        return FAIL_GAME_ENTITY_INIT;
    }

    // TODO initialize notcurses here

    return 0;
}

int main(void) {
    int exit_code = init();
    if (exit_code != 0) {
        return exit_code;
    }
    return run_game();
}
