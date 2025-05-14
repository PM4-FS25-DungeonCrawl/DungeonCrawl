#include "wait_output.h"

#include "../../../logging/logger.h"
#include "../../io_handler.h"
#include "../common/output_handler.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Loading screen message buffer
static char loading_message[256] = "";

/**
 * @brief Draw a loading screen with animation
 *
 * This function displays a loading screen with the specified message and 
 * a simple animation to indicate progress.
 *
 * @param text The message to display on the loading screen
 */
// TODO: Attention multiple pre-declaration! io_handler.h & wait_output.h
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

/**
 * @brief Draw the launch screen with title and animation
 *
 * This function displays the game launch screen with title, version,
 * copyright information, and a loading animation.
 */
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

/**
 * @brief Draw the welcome screen with a message
 *
 * This function displays a welcome message on the screen and prompts
 * the user to press any key to continue. It's typically shown after
 * the launch screen animation completes.
 */
void draw_welcome_screen(void) {
    // Get screen dimensions
    int width, height;
    get_screen_dimensions(&width, &height);

    // Clear the screen
    clear_screen();

    // Draw welcome message
    const char* welcome_msg = "Welcome to Dungeon Crawl! Press any key to continue...";
    int msg_len = strlen(welcome_msg);
    int msg_x = (width - msg_len) / 2;
    int msg_y = height / 2;

    print_text_default(msg_y, msg_x, welcome_msg);

    // Render the frame using centralized IO handler
    render_frame();
}