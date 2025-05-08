#ifndef COMMON_OUTPUT_H
#define COMMON_OUTPUT_H

#include "../colors.h"
#include <notcurses/notcurses.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Initialize the output handler
 * 
 * Sets up the output handling system.
 * This function must be called before any other output functions.
 * 
 * @param nc The Notcurses instance to use for output handling
 * @param stdplane The standard plane to use for output
 * @return true on success, false on failure
 */
bool init_output_handler(struct notcurses* nc, struct ncplane* stdplane);

/**
 * @brief Clear the screen
 * 
 * Clears the entire standard plane.
 * 
 * @param bg_color The background color to fill with (default: COLOR_DEFAULT_BG)
 * @return true on success, false on failure
 */
bool clear_screen(uint32_t bg_color);

/**
 * @brief Print text at a specific position
 * 
 * Renders text at the specified coordinates with the given foreground and background colors.
 * 
 * @param y The Y coordinate (row)
 * @param x The X coordinate (column)
 * @param text The text to render
 * @param fg_color The foreground RGB color (0xRRGGBB)
 * @param bg_color The background RGB color (0xRRGGBB)
 * @return true on success, false on failure
 */
bool print_text(int y, int x, const char* text, uint32_t fg_color, uint32_t bg_color);

/**
 * @brief Print text at a specific position with default colors
 * 
 * Renders text at the specified coordinates with the default colors.
 * 
 * @param y The Y coordinate (row)
 * @param x The X coordinate (column)
 * @param text The text to render
 * @return true on success, false on failure
 */
bool print_text_default(int y, int x, const char* text);

/**
 * @brief Print a menu with selection highlighting
 * 
 * Renders a menu with the specified options, highlighting the selected option.
 * 
 * @param title The menu title
 * @param options Array of option strings
 * @param option_count Number of options
 * @param selected_index Index of the selected option
 * @param y The Y coordinate (row) for the menu start
 * @param x The X coordinate (column) for the menu start
 * @param title_fg Foreground color for the title
 * @param title_bg Background color for the title
 * @param option_fg Foreground color for options
 * @param option_bg Background color for options
 * @param selected_fg Foreground color for selected option
 * @param selected_bg Background color for selected option
 * @return true on success, false on failure
 */
bool print_menu(const char* title, const char** options, int option_count, 
                int selected_index, int y, int x,
                uint32_t title_fg, uint32_t title_bg,
                uint32_t option_fg, uint32_t option_bg,
                uint32_t selected_fg, uint32_t selected_bg);

/**
 * @brief Print a menu with default colors
 * 
 * Renders a menu with the specified options, highlighting the selected option,
 * using default colors.
 * 
 * @param title The menu title
 * @param options Array of option strings
 * @param option_count Number of options
 * @param selected_index Index of the selected option
 * @param y The Y coordinate (row) for the menu start
 * @param x The X coordinate (column) for the menu start
 * @return true on success, false on failure
 */
bool print_menu_default(const char* title, const char** options, int option_count,
                        int selected_index, int y, int x);

/**
 * @brief Draw a box frame
 * 
 * Draws a box with optional title at the specified coordinates.
 * 
 * @param y The Y coordinate (row) for the top-left corner
 * @param x The X coordinate (column) for the top-left corner
 * @param height The height of the box
 * @param width The width of the box
 * @param title Optional title (can be NULL)
 * @param fg_color The foreground RGB color (0xRRGGBB)
 * @param bg_color The background RGB color (0xRRGGBB)
 * @return true on success, false on failure
 */
bool draw_box(int y, int x, int height, int width, const char* title,
              uint32_t fg_color, uint32_t bg_color);

/**
 * @brief Draw a box with default colors
 * 
 * @param y The Y coordinate (row) for the top-left corner
 * @param x The X coordinate (column) for the top-left corner
 * @param height The height of the box
 * @param width The width of the box
 * @param title Optional title (can be NULL)
 * @return true on success, false on failure
 */
bool draw_box_default(int y, int x, int height, int width, const char* title);

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

#endif // COMMON_OUTPUT_H