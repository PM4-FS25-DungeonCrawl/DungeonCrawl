#include "io_handler.h"
#include "../logging/logger.h"
#include "../thread/thread_handler.h"
#include "../game.h"
#include "../common.h"  // Added for COMMON_SUCCESS
#include "input/input_handler.h"
#include "output/common/common_output.h"

#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#ifndef _WIN32
#include <unistd.h> // for usleep
#endif

// Global notcurses instance and standard plane
struct notcurses* nc = NULL;
struct ncplane* stdplane = NULL;

// Global variables to maintain IO state
static volatile bool io_running = false;
static int current_game_state = 0;
static io_state_t current_io_state = IO_STATE_GAME;

// Background loading operation variables
static volatile bool loading_in_progress = false;
static char loading_message[256] = "";
static void (*loading_callback)(void) = NULL;

// Launch screen variables
static volatile bool launch_screen_active = false;
static int launch_screen_duration = 0;
static uint64_t launch_screen_start_time = 0;

// Forward declarations
static void process_io_events_thread(void);
static void loading_thread_function(void);
static void draw_loading_screen(void);
static void draw_launch_screen(void);

int init_io_handler(void) {
    log_msg(INFO, "io_handler", "Starting initialization");

    //initialize the Notcurses instance and standard plane
    notcurses_options ncopt;
    memset(&ncopt, 0, sizeof(ncopt));

    log_msg(INFO, "io_handler", "Initializing notcurses");
    nc = notcurses_init(&ncopt, stdout);
    if (nc == NULL) {
        log_msg(ERROR, "io_handler", "Failed to initialize notcurses");
        return 1; // Error code
    }

    log_msg(INFO, "io_handler", "Getting standard plane");
    stdplane = notcurses_stdplane(nc);
    if (!stdplane) {
        log_msg(ERROR, "io_handler", "Failed to get standard plane");
        notcurses_stop(nc);
        nc = NULL;
        return 2; // Error code
    }

    ncplane_set_bg_rgb(stdplane, 0x281D10);

    // Initialize input handler (which starts its own thread)
    if (!init_input_handler(nc)) {
        log_msg(ERROR, "io_handler", "Failed to initialize input handler");
        return 3; // Error code
    }

    // Initialize common output handler (which starts its own thread)
    if (!init_output_handler()) {
        log_msg(ERROR, "io_handler", "Failed to initialize output handler");
        shutdown_input_handler();
        return 4; // Error code
    }

    // Start the IO event processing thread
    io_running = true;
    start_simple_thread(process_io_events_thread);

    log_msg(INFO, "io_handler", "IO handler initialized successfully with multithreading");
    return COMMON_SUCCESS; // 0
}

// Loading screen thread function
static void loading_thread_function(void) {
    log_msg(INFO, "io_handler", "Loading thread started");

    // Call the user-provided loading callback
    if (loading_callback) {
        loading_callback();
    }

    // When loading is complete, go back to game state
    loading_in_progress = false;
    current_io_state = IO_STATE_GAME;

    log_msg(INFO, "io_handler", "Loading thread completed");
}

// Draw the loading screen with animation
static void draw_loading_screen(void) {
    if (!stdplane) return;

    // Get screen dimensions
    int width, height;
    get_screen_dimensions(&width, &height);

    // Clear the screen
    clear_screen();

    // Draw loading message
    int msg_len = strlen(loading_message);
    int msg_x = (width - msg_len) / 2;
    int msg_y = height / 2 - 1;

    print_text(msg_y, msg_x, loading_message, DEFAULT_COLORS);

    // Draw a simple loading animation based on the current time
    static int frame = 0;
    frame = (frame + 1) % 4;

    char anim[5] = "|/-\\";
    char animation_str[2] = {anim[frame], '\0'};

    print_text(msg_y + 2, width / 2, animation_str, DEFAULT_COLORS);

    // Render the frame
    render_frame();
}

// Draw the launch screen with title and animation
static void draw_launch_screen(void) {
    if (!stdplane) return;

    // Get screen dimensions
    int width, height;
    get_screen_dimensions(&width, &height);

    // Clear the screen
    clear_screen();

    // Draw game title
    const char* title = "DUNGEON CRAWL";
    int title_len = strlen(title);
    int title_x = (width - title_len) / 2;
    int title_y = height / 3;

    // Draw the title with a special color
    print_text(title_y, title_x, title, RED_TEXT_COLORS);

    // Draw version and copyright
    const char* version = "Version 1.0";
    const char* copyright = "(C) 2025 DungeonCrawl Team";

    int version_len = strlen(version);
    int copyright_len = strlen(copyright);

    int version_x = (width - version_len) / 2;
    int copyright_x = (width - copyright_len) / 2;

    print_text(title_y + 2, version_x, version, DEFAULT_COLORS);
    print_text(title_y + 3, copyright_x, copyright, DEFAULT_COLORS);

    // Draw a "Press any key to continue" message if close to the end
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    uint64_t elapsed_ms = (current_time.tv_sec * 1000 + current_time.tv_nsec / 1000000) - launch_screen_start_time;

    // Only show the "press any key" message in the last second
    if (elapsed_ms >= launch_screen_duration - 1000) {
        const char* press_key = "Press any key to continue...";
        int key_len = strlen(press_key);
        int key_x = (width - key_len) / 2;

        print_text(height - 5, key_x, press_key, DEFAULT_COLORS);
    }

    // Render the frame
    render_frame();
}

// Main IO event processing thread function
static void process_io_events_thread(void) {
    log_msg(INFO, "io_handler", "IO event processing thread started");

    while (io_running) {
        // Process based on current IO state
        switch (current_io_state) {
            case IO_STATE_LOADING:
                // Draw the loading screen
                draw_loading_screen();
                break;

            case IO_STATE_LAUNCH:
                // Draw the launch screen with game logo
                draw_launch_screen();

                // Check if the launch screen duration has elapsed
                struct timespec current_time;
                clock_gettime(CLOCK_MONOTONIC, &current_time);
                uint64_t elapsed_ms = (current_time.tv_sec * 1000 + current_time.tv_nsec / 1000000) - launch_screen_start_time;

                if (elapsed_ms >= launch_screen_duration) {
                    // Launch screen duration has elapsed, go back to game mode
                    current_io_state = IO_STATE_GAME;
                    launch_screen_active = false;
                }
                break;

            case IO_STATE_GAME:
                // Normal game IO processing
                // We don't need to do much here as the game loop handles the actual rendering
                break;
        }

        // Check for input events (regardless of IO state)
        input_event_t event;
        if (get_input_nonblocking(&event)) {
            // Quit event is handled in all states
            if (event.type == INPUT_QUIT) {
                io_running = false;
            }
        }

        // Small sleep to avoid using 100% CPU
        #ifdef _WIN32
            Sleep(10); // 10 milliseconds
        #else
            usleep(10000); // 10 milliseconds
        #endif
    }

    log_msg(INFO, "io_handler", "IO event processing thread stopped");
}

// Show a loading screen and execute a callback in the background
bool show_loading_screen(const char* text, void (*callback)(void)) {
    if (!text || !callback) {
        log_msg(ERROR, "io_handler", "Invalid parameters for loading screen");
        return false;
    }

    // Store the loading parameters
    strncpy(loading_message, text, sizeof(loading_message) - 1);
    loading_message[sizeof(loading_message) - 1] = '\0';
    loading_callback = callback;

    // Set state and flags
    current_io_state = IO_STATE_LOADING;
    loading_in_progress = true;

    // Start the loading thread
    start_simple_thread(loading_thread_function);

    return true;
}

// Show the game launch screen for the specified duration
bool show_launch_screen(int duration_ms) {
    if (duration_ms <= 0) {
        log_msg(ERROR, "io_handler", "Invalid duration for launch screen");
        return false;
    }

    // Set up the launch screen
    launch_screen_duration = duration_ms;
    launch_screen_active = true;
    current_io_state = IO_STATE_LAUNCH;

    // Record the start time
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    launch_screen_start_time = start_time.tv_sec * 1000 + start_time.tv_nsec / 1000000;

    return true;
}

// Process input events and update game state
// This function is called from the main game loop
int process_io_events(int game_state) {
    // Update the current game state for the IO thread
    current_game_state = game_state;

    // If we're in a loading state, don't do normal game processing
    if (current_io_state != IO_STATE_GAME) {
        // We're showing a loading screen or launch screen

        // Check for any key press to skip the launch screen
        input_event_t event;
        if (current_io_state == IO_STATE_LAUNCH && get_input_nonblocking(&event)) {
            // Any key press skips the launch screen
            current_io_state = IO_STATE_GAME;
            launch_screen_active = false;
        }

        // Return the current game state (no change)
        return current_game_state;
    }

    // Process input for the game state
    input_event_t event;
    if (get_input_nonblocking(&event)) {
        // Handle the input event based on the current game state
        switch (current_game_state) {
            case MAIN_MENU:
                // Handle main menu input
                if (event.type == INPUT_QUIT) {
                    return EXIT; // Game should exit
                }
                break;

            case MAP_MODE:
                // Handle map input
                if (event.type == INPUT_QUIT) {
                    return EXIT;
                } else if (event.type == INPUT_MENU) {
                    return MAIN_MENU;
                } else if (event.type == INPUT_STATS) {
                    return STATS_MODE;
                }
                break;

            // Add other game states as needed

            default:
                // Default handling
                if (event.type == INPUT_QUIT) {
                    return EXIT;
                }
                break;
        }
    }

    // Return the current game state (no change from input)
    return current_game_state;
}

void shutdown_io_handler(void) {
    // Stop the IO event processing thread
    io_running = false;

    // Allow the thread some time to clean up
    #ifdef _WIN32
        Sleep(100);
    #else
        usleep(100000);
    #endif

    // Shutdown subsystems in reverse order of initialization
    shutdown_output_handler();
    shutdown_input_handler();

    // Shutdown notcurses
    if (nc) {
        notcurses_stop(nc);
        nc = NULL;
        stdplane = NULL;
    }

    log_msg(INFO, "io_handler", "IO handler shut down");
}