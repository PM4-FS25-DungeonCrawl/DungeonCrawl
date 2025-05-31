/**
 * @file main.c
 * @brief Entry point handling game initialization, launch screen display, main loop start, and shutdown.
 */
#include "main.h"

#include "combat/combat_mode.h"
#include "combat/local/ability_local.h"
#include "common.h"
#include "database/game/gamestate_database.h"
#include "game.h"
#include "game_data.h"
#include "inventory/inventory_mode.h"
#include "io/io_handler.h"
#include "io/output/specific/stats_output.h"
#include "local/local_handler.h"
#include "logging/logger.h"
#include "map/local/map_mode_local.h"
#include "map/map_mode.h"
#include "menu/language_menu.h"
#include "menu/main_menu.h"
#include "menu/save_menu.h"
#include "item/local/gear_local.h"

#ifndef _WIN32
    #include <unistd.h>// for usleep
#endif

#include "io/output/specific/wait_output.h"

#include <time.h>

// Global flag to signal when initialization is complete
volatile int init_done = 0;

/**
 * @brief Display the launch screen.
 */
static void display_launch_screen_thread(void) {
    clear_screen();

    // Track if we've displayed for minimum time
    time_t start_time = time(NULL);
    bool min_time_elapsed = false;

    // Draw the launch screen for LAUNCH_SCREEN_MIN_DISPLAY_TIME_MS milliseconds
    while (!min_time_elapsed) {
        // Call the draw_launch_screen function from wait_output.c
        draw_launch_screen();

        // Check if we can exit the loop (both conditions met)
        time_t current_time = time(NULL);
        double elapsed_s = difftime(current_time, start_time);
        long elapsed_ms = (long) (elapsed_s * 1000);

        min_time_elapsed = (elapsed_ms >= LAUNCH_SCREEN_MIN_DISPLAY_TIME_MS);
    }

    // Clear the screen and render one more time to ensure it's cleared
    clear_screen();
    render_frame();

    draw_welcome_screen();
}

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

    // First initialize the IO handler (needed for the splash screen)
    if (init_io_handler() != COMMON_SUCCESS) {
        log_msg(ERROR, "Main", "Failed to initialize IO handler");
        return FAIL_IO_HANDLER_INIT;
    }

    // Start the launch screen in a background thread
    // don't need multithreading anymore, init is too fast
    display_launch_screen_thread();


    // Initialize database connection
    if (db_open_multiple_access(&db_connection, DB_GAME) != DB_OPEN_STATUS_SUCCESS) {
        log_msg(ERROR, "Game", "Failed to open database");
        return 1;
    }
    create_tables_game_state(&db_connection);

    // Initialize localization
    if (init_local_handler(LANGE_EN) != COMMON_SUCCESS) {
        log_msg(ERROR, "Main", "Failed to initialize local handler");
        return FAIL_LOCAL_INIT;
    }

    // Initialize map mode
    init_map_mode();
    // the local modul for map mode
    if (init_map_mode_local() != COMMON_SUCCESS) return FAIL_MAP_MODE_LOCAL_INIT;

    // Initialize main menu
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
    if (init_ability_local() != COMMON_SUCCESS) return FAIL_ABILITY_LOCAL_INIT;
    if (init_gear_local() != COMMON_SUCCESS) return FAIL_GEAR_LOCAL_INIT;

    // When all initialization is done, switch back to game mode
    init_done = 1;// Set the global flag to signal completion
    return COMMON_SUCCESS;
}

/**
 * @brief Shuts down the entire game and frees associated resources.
 */
void shutdown_game() {
    free_game_data();
    // close database connection in game.c
    db_close(&db_connection);

    shutdown_gear_local();
    shutdown_ability_local();
    shutdown_map_mode();
    shutdown_map_mode_local();
    shutdown_combat_mode();
    shutdown_stats_mode();
    shutdown_inventory_mode();
    shutdown_language_menu();
    shutdown_save_menu();
    shutdown_main_menu();

    shutdown_local_handler();
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
