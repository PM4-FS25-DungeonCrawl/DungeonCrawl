/**
 * @file output_handler.h
 * @brief Exposes functions for outputting to the console.
 */
#ifndef OUTPUT_HANDLER_H
#define OUTPUT_HANDLER_H

#include <notcurses/notcurses.h>
#include <stdbool.h>

/**
 * @brief Initialize the output handler
 *
 * Sets up the output handling system.
 * This function must be called before any other output functions.
 *
 * @return true on success, false on failure
 */
bool init_output_handler(void);

/**
 * @brief Clear the screen
 *
 * Clears the entire standard plane.
 * TODO: this should be split into different functions, depending on whether we need to clear all planes, the standard plane, or a specific plane.
 */
void clear_screen(void);

/**
 * @brief Render the current frame
 * 
 * Renders all changes to the screen.
 * 
 * @return true on success, false on failure
 */
bool render_frame(void);

/**
 * @brief Get the dimensions of the standard plane
 * 
 * @param[out] width Pointer to store the width
 * @param[out] height Pointer to store the height
 * @return true on success, false on failure
 */
bool get_screen_dimensions(int* width, int* height);

/**
 * @brief Shutdown the output handler
 * 
 * Cleans up resources.
 */
void shutdown_output_handler(void);

#endif// OUTPUT_HANDLER_H
