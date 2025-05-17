#ifndef COMMON_OUTPUT_H
#define COMMON_OUTPUT_H

#include <notcurses/notcurses.h>

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
 * @brief Print text at a specific position
 * 
 * Renders text at the specified coordinates with the given foreground and background colors.
 * 
 * @param y The Y coordinate (row)
 * @param x The X coordinate (column)
 * @param text The text to render
 * @param ncchannel The color channel (foreground and background)
 * @return true on success, false on failure
 */
void print_text(int y, int x, const char* text, uint64_t ncchannel);

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
void print_text_default(int y, int x, const char* text);

void print_text_multi_line(int y, int x, const char* text, int max_width, u_int64_t ncchannel);

void print_text_multi_line_default(int y, int x, const char* text, int max_width);

//TODO: add a function that let's you pass multiple strings to print!!
void print_text_multi_strings(int y, int x, const char* text[], int count, uint64_t ncchannel);

void print_text_multi_strings_default(int y, int x, const char* text[], int count);

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
 * @param title_channel The color channel for the title
 * @param option_channel The color channel for the options
 * @param selected_channel The color channel for the selected option
 * @return true on success, false on failure
 */
void print_menu(const char* title, const char** options, int option_count,
                int selected_index, int y, int x,
                uint64_t title_channel,
                uint64_t option_channel,
                uint64_t selected_channel);

void print_menu_default(const char* title, const char** options, int option_count,
                        int selected_index, int y, int x);


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

#endif// COMMON_OUTPUT_H