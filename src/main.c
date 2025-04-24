#include "main.h"

#include "../include/termbox2.h"
#include "combat/combat_mode.h"
#include "common.h"
#include "game.h"
#include "game_data.h"
#include "local/local.h"
#include "logging/logger.h"
#include "map/map_mode.h"
#include "menu/main_menu.h"

#include <time.h>

// === Global Variables ===
memory_pool_t* main_memory_pool;

/**
 * Frees all allocated resources and performs cleanup tasks for the game.
 * This function should be called before the program exits to ensure proper
 * shutdown of all modules, including logging, local settings, and game data.
 */
void shutdown(void);

int init() {
    // TODO: remove after notcurses switch
    if (tb_init() != 0) {
        log_msg(ERROR, "Game", "Failed to initialize termbox");
        return FAIL_TB_INIT;
    }
    tb_set_output_mode(TB_OUTPUT_NORMAL);

    // seeding random function
    srand(time(NULL));

    init_logger();

    // Initialize database connection
    if (db_open(&db_connection, "resources/database/game/dungeoncrawl_game.db") != DB_OPEN_STATUS_SUCCESS) {
        log_msg(ERROR, "Game", "Failed to open database");
        return 1;
    }
    create_tables_game_state(&db_connection);// only for dungeoncrawl_game.db

    main_memory_pool = init_memory_pool(STANDARD_MEMORY_POOL_SIZE);
    if (main_memory_pool == NULL) {
        log_msg(ERROR, "Main", "Failed to initialize memory pool");
        return FAIL_MEM_POOL_INIT;
    }
    if (init_local() != COMMON_SUCCESS) {
        log_msg(ERROR, "Main", "Failed to initialize local");
        return FAIL_LOCAL_INIT;
    }

    init_map_mode();
    init_main_menu();
    if (init_combat_mode(main_memory_pool) != COMMON_SUCCESS) {
        log_msg(ERROR, "Main", "Failed to initialize combat mode");
        return FAIL_GAME_MODE_INIT;
    }
    if (init_game_data(main_memory_pool) != COMMON_SUCCESS) {
        log_msg(ERROR, "Game", "Failed to initialize game components");
        return FAIL_GAME_ENTITY_INIT;
    }

    // TODO initialize notcurses here

    return 0;
}

void shutdown() {
    free_game_data(main_memory_pool);
    shutdown_local();
    // close database connection in game.c
    db_close(&db_connection);

    shutdown_combat_mode(main_memory_pool);
    shutdown_memory_pool(main_memory_pool);

    shutdown_logger();
    tb_shutdown();
}

int main(void) {
    const int exit_code = init();
    if (exit_code != COMMON_SUCCESS) {
        shutdown();
        return exit_code;
    }
    run_game();
    shutdown();
    return exit_code;
}
