#include "main.h"

#include "combat/combat_mode.h"
#include "game.h"
#include "game_data.h"
#include "local/local.h"
#include "logging/logger.h"
#include "map/map_mode.h"
#include "menu/main_menu.h"
#include "notcurses/notcurses.h"
#include "src/database/gamestate/gamestate_database.h"

#include <time.h>

/**
 * Frees all allocated resources and performs cleanup tasks for the game.
 * This function should be called before the program exits to ensure proper
 * shutdown of all modules, including logging, local settings, and game data.
 */
void shutdown_game(void);

int init() {
    // seeding random function
    srand(time(NULL));

    init_logger();

    // Initialize database connection
    if (db_open(&db_connection, "../resources/database/game/dungeoncrawl_game.db") != DB_OPEN_STATUS_SUCCESS) {
        log_msg(ERROR, "Game", "Failed to open database");
        return 1;
    }
    create_tables_game_state(&db_connection);// only for dungeoncrawl_game.db

    if (init_local() != 0) {
        log_msg(ERROR, "Main", "Failed to initialize local");
        return FAIL_LOCAL_INIT;
    }

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

void shutdown_game() {
    free_game_data();
    shutdown_local();
    // close database connection in game.c
    db_close(&db_connection);

    shutdown_combat_mode();
    shutdown_logger();
    notcurses_stop(nc);
}

int main(void) {
    int exit_code = init();
    if (exit_code != 0) {
        shutdown_game();
        return exit_code;
    }
    run_game();
    shutdown_game();
    return exit_code;
}
