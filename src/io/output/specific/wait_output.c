#include "wait_output.h"

#include "../../../logging/logger.h"
#include "../../io_handler.h"
#include "../common/common_output.h"
#include "../media/media_output.h"  // Added for media support

#include <stdio.h>
#include <string.h>

// Include platform-specific headers for sleep functions
#ifndef _WIN32
    #include <unistd.h>  // For usleep on Unix/Linux
#else
    #include <windows.h> // For Sleep on Windows
#endif

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
    render_io_frame();
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

    // Load the goblin image once during initialization
    static loaded_visual_t* goblin_img = NULL;
    static bool image_loaded = false;

    if (!image_loaded) {
        log_msg(INFO, "Wait Output", "Loading goblin image for launch screen");
        const char* image_path = "/home/jil/DungeonCrawl/src/art/goblin.png";
        int img_width, img_height;
        goblin_img = load_image(image_path, &img_width, &img_height);
        image_loaded = true;

        if (goblin_img) {
            log_msg(INFO, "Wait Output", "Successfully loaded goblin image (%dx%d)", img_width, img_height);
        } else {
            log_msg(ERROR, "Wait Output", "Failed to load goblin image");
        }
    }

    // Draw the goblin image if loaded
    if (goblin_img) {
        // Position the image below the title
        int image_y = title_y + 5;
        int image_width = 5;  // Terminal cells, not pixels
        int image_height = 5; // Terminal cells, not pixels
        int image_x = (width - image_width) / 2;

        log_msg(INFO, "Wait Output", "Displaying goblin at position (%d, %d), size %dx%d",
                image_x, image_y, image_width, image_height);

        // Display the image
        bool display_result = display_image_positioned(goblin_img, image_y, image_x, image_width, image_height);
        if (!display_result) {
            log_msg(ERROR, "Wait Output", "Failed to display goblin image");
        }
    }

    // Draw a loading message
    const char* loading_msg = "Loading game...";
    int loading_len = strlen(loading_msg);

    // Continue showing animation until init is done
    extern volatile int init_done;
    while (!init_done) {
        // Show simple animation
        static int frame = 0;
        frame = (frame + 1) % 4;
        char anim[5] = "|/-\\";
        char animation_str[32];
        snprintf(animation_str, sizeof(animation_str), "%s %c", loading_msg, anim[frame]);

        print_text_default(height - 5, (width - loading_len - 2) / 2, animation_str);

        // Render the frame using centralized IO handler
        render_io_frame();

        // Pause briefly to prevent 100% CPU usage and to make animation visible
#ifdef _WIN32
        Sleep(100); // 100ms pause on Windows
#else
        usleep(100000); // 100ms pause on Unix/Linux
#endif
    }

    // Free the goblin image when we're done with the launch screen
    if (goblin_img) {
        log_msg(INFO, "Wait Output", "Freeing goblin image as initialization completed");
        free_visual(goblin_img);
        goblin_img = NULL;
        // Clear the image off the screen
        clear_screen();
        image_loaded = false;
    }
}