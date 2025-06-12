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
 */
void clear_screen(void);

/**
 * @brief Clear all planes in the display stack
 *
 * Clears all visible planes, including media and overlay planes.
 */
void clear_all_planes(void);

/**
 * @brief Clear the standard plane only
 *
 * Clears only the standard plane, leaving other planes intact.
 */
void clear_standard_plane(void);

/**
 * @brief Clear a specific plane
 *
 * Clears the specified plane.
 * @param plane Pointer to the plane to clear
 */
void clear_specific_plane(struct ncplane* plane);

/**
 * @brief Handle screen resize event
 *
 * Refreshes the display and redraws content after a terminal resize.
 * @return true on success, false on failure
 */
bool handle_screen_resize(void);

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

#endif// OUTPUT_HANDLER_H
