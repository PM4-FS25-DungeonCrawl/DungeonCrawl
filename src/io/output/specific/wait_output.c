/**
 * @file wait_output.c
 * @brief Implementation for drawing the loading screen.
 */
#include "wait_output.h"

#include "../../../game_data.h"
#include "../../../logging/logger.h"
#include "../../io_handler.h"
#include "../common/output_handler.h"
#include "../common/text_output.h"
#include "../media/media_files.h"
#include "../media/media_output.h"

#include <stdio.h>
#include <string.h>

// Include platform-specific headers for sleep functions
#ifndef _WIN32
    #include <unistd.h>// For usleep on Unix/Linux
#else
    #include <windows.h>// For Sleep on Windows
#endif

// Loading screen message buffer
static char loading_message[256] = "";


void draw_loading_screen(const char* text) {
    if (!text) {
        log_msg(ERROR, "Wait Output", "Loading screen text is NULL");
        return;
    }

    // Store the message
    strncpy(loading_message, text, sizeof(loading_message) - 1);
    loading_message[sizeof(loading_message) - 1] = '\0';

    // Get screen dimensions
    int width, height;
    get_screen_dimensions(&width, &height);

    // Clear the screen
    clear_screen();

    // Draw loading message
    int msg_len = strlen(loading_message);
    int msg_x = (width - msg_len) / 2;
    int msg_y = height / 2 - 1;

    print_text_default(msg_y, msg_x, loading_message);

    // Draw a simple loading animation based on the current frame
    static int frame = 0;
    frame = (frame + 1) % 4;

    char anim[10] = "|/-\\|/-\\";
    char animation_str[2] = {anim[frame], '\0'};

    print_text_default(msg_y + 2, width / 2, animation_str);

    // Render the frame using centralized IO handler
    render_frame();
}


void draw_launch_screen(void) {
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

    print_text_default(title_y + 2, version_x, version);
    print_text_default(title_y + 3, copyright_x, copyright);

    // Draw a loading message
    const char* loading_msg = "Loading game...";
    int loading_len = strlen(loading_msg);

    // Show simple animation
    static int frame = 0;
    frame = (frame + 1) % 4;
    char anim[5] = "|/-\\";
    char animation_str[32];
    snprintf(animation_str, sizeof(animation_str), "%s %c", loading_msg, anim[frame]);

    print_text_default(height - 5, (width - loading_len - 2) / 2, animation_str);

    // Render the frame using centralized IO handler
    render_frame();
// Sleep for a short duration to control the animation speed
#ifdef _WIN32
    Sleep(50);
#else
    usleep(50000);// 50ms
#endif
}


void draw_welcome_screen(void) {
    // Get screen dimensions
    int width, height;
    get_screen_dimensions(&width, &height);

    // Clear the screen
    clear_screen();

    // Create personalized welcome message
    char welcome_msg[256];
    const char* player_name = get_player_name();
    snprintf(welcome_msg, sizeof(welcome_msg), "Welcome to Dungeon Crawl, %s! Press any key to continue...", player_name);

    int msg_len = strlen(welcome_msg);
    int msg_x = (width - msg_len) / 2;
    int msg_y = height / 4;

    print_text_default(msg_y, msg_x, welcome_msg);

    // Render the frame using centralized IO handler
    render_frame();

    display_gif_at(INTRO_GIF, (width - INTRO_WIDTH) / 2, msg_y + 5, INTRO_HEIGHT, INTRO_WIDTH, SCALE_STRETCH, 5, true);

    // Wait for user input
    input_event_t input_event;
    get_input_blocking(&input_event);

    // Clear the screen after input
    media_cleanup();
}
