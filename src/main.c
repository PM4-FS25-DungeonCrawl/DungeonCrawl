#include "main.h"

#include "combat/combat_mode.h"
#include "common.h"
#include "database/game/gamestate_database.h"
#include "game.h"
#include "game_data.h"
#include "inventory/inventory_mode.h"
#include "local/local_handler.h"
#include "logging/logger.h"
#include "map/map_mode.h"
#include "menu/language_menu.h"
#include "menu/main_menu.h"
#include "menu/save_menu.h"
#include "stats/draw/draw_stats.h"

#include <notcurses/notcurses.h>
#include <time.h>

/**
 * Initializes all necessary parts and subsystems for the game.
 * This function sets up the logger, memory pool, database connection,
 * game modes, and other required modules. It also performs any necessary
 * seeding or initialization tasks needed for the game to run correctly.
 *
 * @return An exit code indicating success or the specific failure that occurred.
 */
int init(void);
/**
 * Frees all allocated resources and performs cleanup tasks for the game.
 * This function should be called before the program exits to ensure proper
 * shutdown of all modules, including logging, local settings, and game data.
 */
void shutdown_game(void);

int init() {
    // initialized the main memory pool
    main_memory_pool = init_memory_pool(STANDARD_MEMORY_POOL_SIZE);
    NULL_PTR_HANDLER_RETURN(main_memory_pool, FAIL_MEM_POOL_INIT, "Main", "Main memory pool is NULL");

    // seeding random function
    srand(time(NULL));

    init_logger();

    // Initialize database connection
    if (db_open(&db_connection, "resources/database/game/dungeoncrawl_game.db") != DB_OPEN_STATUS_SUCCESS) {
        log_msg(ERROR, "Game", "Failed to open database");
        return 1;
    }
    create_tables_game_state(&db_connection);// only for dungeoncrawl_game.db

    if (init_local_handler(LANGE_EN) != COMMON_SUCCESS) {
        log_msg(ERROR, "Main", "Failed to initialize local handler");
        return FAIL_LOCAL_INIT;
    }
    init_map_mode();
    if (init_main_menu() != COMMON_SUCCESS) {
        log_msg(ERROR, "Main", "Failed to initialize main menu");
        return FAIL_MAIN_MENU_INIT;
    }
    if (init_save_menu() != COMMON_SUCCESS) {
        log_msg(ERROR, "Main", "Failed to initialize save menu");
        return FAIL_SAVE_MENU_INIT;
    }
    if (init_language_menu() != COMMON_SUCCESS) {
        log_msg(ERROR, "Main", "Failed to initialize language menu");
        return FAIL_LANGUAGE_INIT;
    }
    if (init_combat_mode() != COMMON_SUCCESS) {
        log_msg(ERROR, "Main", "Failed to initialize combat mode");
        return FAIL_GAME_MODE_INIT;
    }
    if (init_inventory_mode() != COMMON_SUCCESS) {
        log_msg(ERROR, "Main", "Failed to initialize inventory mode");
        return FAIL_INVENTORY_MODE_INIT;
    }
    if (init_game_data() != COMMON_SUCCESS) {
        log_msg(ERROR, "Game", "Failed to initialize game components");
        return FAIL_GAME_ENTITY_INIT;
    }
    if (init_stats_mode() != COMMON_SUCCESS) {
        log_msg(ERROR, "Stats", "Failed to intialize stats components");
        return FAIL_STATS_MODE_INIT;
    }

    // TODO initialize notcurses here

    return 0;
}

void shutdown_game() {
    free_game_data();
    shutdown_local_handler();
    // close database connection in game.c
    db_close(&db_connection);

    shutdown_combat_mode();
    shutdown_stats_mode();
    shutdown_inventory_mode();
    shutdown_language_menu();
    shutdown_save_menu();
    shutdown_main_menu();

    //shutdown the main memory pool
    shutdown_memory_pool(main_memory_pool);
    shutdown_logger();
    notcurses_stop(nc);
}

int main(void) {
    const int exit_code = init();
    if (exit_code != COMMON_SUCCESS) {
        shutdown_game();
        return exit_code;
    }
    run_game();
    shutdown_game();
    return exit_code;
}
