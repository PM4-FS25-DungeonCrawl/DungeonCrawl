#ifndef WAIT_OUTPUT_H
#define WAIT_OUTPUT_H

/**
 * @brief Draw a loading screen with animation
 *
 * @param text The message to display on the loading screen
 */
void draw_loading_screen(const char* text);

/**
 * @brief Draw the launch screen with title and animation
 */
void draw_launch_screen(void);

/**
 * @brief Draw the welcome screen with a message
 */
void draw_welcome_screen(void);

#endif// WAIT_OUTPUT_H