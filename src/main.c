#include "main.h"

#include "combat/combat_mode.h"
#include "common.h"
#include "database/gamestate/gamestate_database.h"
#include "game.h"
#include "game_data.h"
#include "inventory/inventory_mode.h"
#include "local/local.h"
#include "logging/logger.h"
#include "map/map_mode.h"
#include "menu/main_menu.h"
#include "notcurses/notcurses.h"
#include "src/database/gamestate/gamestate_database.h"
#include "stats/draw/draw_stats.h"
#include "io/io_handler.h"

#include <time.h>

/**
 * Frees all allocated resources and performs cleanup tasks for the game.
 */
void shutdown_game(void);

/**
 * Initializes all necessary parts and subsystems for the game.
 * First initializes IO handler to show a splash screen, then initializes
 * the rest of the systems sequentially.
 *
 * @return An exit code indicating success or the specific failure that occurred.
 */
int init() {
    // Initialize the main memory pool
    main_memory_pool = init_memory_pool(STANDARD_MEMORY_POOL_SIZE);
    NULL_PTR_HANDLER_RETURN(main_memory_pool, FAIL_MEM_POOL_INIT, "Main", "Main memory pool is NULL");

    // Seed random function
    srand(time(NULL));

    // Initialize logger
    init_logger();
    log_msg(INFO, "Main", "Initialization started");

    // First initialize the IO handler (needed for the splash screen)
    if (init_io_handler() != COMMON_SUCCESS) {
        log_msg(ERROR, "Main", "Failed to initialize IO handler");
        return FAIL_IO_HANDLER_INIT;
    }

    
    //TODO: handle threading here! create a thread that displays the loading screen, and is terminated as soon as the rest of the game is initialized
    // Draw the launch screen and display while initializing (this is a placeholder, we need multithreading)
    int init_done = 0;
    while (!init_done) {
        draw_launch_screen();
        usleep(100000); // Sleep for 0.1 seconds
        if (init_done) {
            break;
        }
    }

    // Initialize database connection
    if (db_open(&db_connection, "resources/database/game/dungeoncrawl_game.db") != DB_OPEN_STATUS_SUCCESS) {
        log_msg(ERROR, "Game", "Failed to open database");
        return 1;
    }
    create_tables_game_state(&db_connection);

    // Initialize localization
    if (init_local() != COMMON_SUCCESS) {
        log_msg(ERROR, "Main", "Failed to initialize local");
        return FAIL_LOCAL_INIT;
    }

    // Initialize map mode
    init_map_mode();

    // Initialize main menu
    if (init_main_menu() != COMMON_SUCCESS) {
        log_msg(ERROR, "Main", "Failed to initialize main menu");
        return FAIL_MAIN_MENU_INIT;
    }

    // Initialize combat mode
    if (init_combat_mode() != COMMON_SUCCESS) {
        log_msg(ERROR, "Main", "Failed to initialize combat mode");
        return FAIL_GAME_MODE_INIT;
    }

    // Initialize inventory mode
    if (init_inventory_mode() != COMMON_SUCCESS) {
        log_msg(ERROR, "Main", "Failed to initialize inventory mode");
        return FAIL_INVENTORY_MODE_INIT;
    }

    // Initialize game data
    if (init_game_data() != COMMON_SUCCESS) {
        log_msg(ERROR, "Game", "Failed to initialize game components");
        return FAIL_GAME_ENTITY_INIT;
    }

    // Initialize stats mode
    if (init_stats_mode() != COMMON_SUCCESS) {
        log_msg(ERROR, "Stats", "Failed to initialize stats components");
        return FAIL_STATS_MODE_INIT;
    }

    // When all initialization is done, switch back to game mode
    log_msg(INFO, "Main", "Initialization complete");
    int init_done = 1;
    return COMMON_SUCCESS;
}

void shutdown_game() {
    free_game_data();
    shutdown_local();
    // close database connection in game.c
    db_close(&db_connection);

    shutdown_map_mode();
    shutdown_combat_mode();
    shutdown_stats_mode();
    shutdown_inventory_mode();

    //shutdown the main memory pool
    shutdown_memory_pool(main_memory_pool);
    shutdown_logger();
    shutdown_io_handler();
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
