#include "wait_output.h"
#include "../common/common_output.h"
#include "../../input/input_handler.h"
#include "../../io_handler.h"
#include "../../../game.h"
#include "../../../logging/logger.h"
#include <notcurses/notcurses.h>
#include <string.h>
#include <stdio.h>

// Reference to the global notcurses instance and standard plane
extern struct notcurses* nc;
extern struct ncplane* stdplane;

// Loading screen message buffer
static char loading_message[256] = "";

// Draw the loading screen with animation
void draw_loading_screen(const char* text) {
    if (!stdplane || !text) return;

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
void draw_launch_screen(void) {
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

    // Draw a loading message
    const char* loading_msg = "Loading game...";
    int loading_len = strlen(loading_msg);

    // Show simple animation
    static int frame = 0;
    frame = (frame + 1) % 4;
    char anim[5] = "|/-\\";
    char animation_str[32];
    snprintf(animation_str, sizeof(animation_str), "%s %c", loading_msg, anim[frame]);

    print_text(height - 5, (width - loading_len - 2) / 2, animation_str, DEFAULT_COLORS);

    // Render the frame
    render_frame();
}