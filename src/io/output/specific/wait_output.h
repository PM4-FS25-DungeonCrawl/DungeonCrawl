/**
 * @file wait_output.h
 * @brief Exposes functions for drawing the laoding screen.
 */
#ifndef WAIT_OUTPUT_H
#define WAIT_OUTPUT_H

// Define constants for display timing
#define LAUNCH_SCREEN_MIN_DISPLAY_TIME_MS 2000// Minimum time to display launch screen (2 seconds)

/**
 * @brief Draw a loading screen with animation
 *
 * This function displays a loading screen with the specified message and
 * a simple animation to indicate progress.
 *
 * @param text The message to display on the loading screen
 */
void draw_loading_screen(const char* text);

/**
 * @brief Draw the launch screen with title and animation
 *
 * This function displays the game launch screen with title, version,
 * copyright information, and a loading animation.
 */
void draw_launch_screen(void);

/**
 * @brief Draw the welcome screen with a message
 *
 * This function displays a welcome message on the screen and prompts
 * the user to press any key to continue. It's typically shown after
 * the launch screen animation completes.
 */
void draw_welcome_screen(void);

#endif// WAIT_OUTPUT_H
