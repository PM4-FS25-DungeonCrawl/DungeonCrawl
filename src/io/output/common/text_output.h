#ifndef TEXT_OUTPUT_H
#define TEXT_OUTPUT_H

#include <notcurses/notcurses.h>
#include <stdbool.h>

/**
 * @brief Text alignment options
 */
typedef enum text_align {
    TEXT_ALIGN_LEFT = NCALIGN_LEFT,
    TEXT_ALIGN_CENTER = NCALIGN_CENTER,
    TEXT_ALIGN_RIGHT = NCALIGN_RIGHT
} text_align_t;

/**
 * @brief Text style options
 */
typedef enum text_style {
    TEXT_STYLE_NORMAL   = 0,
    TEXT_STYLE_BOLD     = NCSTYLE_BOLD,
    TEXT_STYLE_ITALIC   = NCSTYLE_ITALIC,
    TEXT_STYLE_UNDERLINE = NCSTYLE_UNDERLINE
} text_style_t;

/**
 * @brief Text transparency options
 */
typedef enum text_transparency {
    TEXT_TRANSPARENCY_NONE = 0,         // Fully opaque
    TEXT_TRANSPARENCY_TEXT_ONLY,        // Only text visible, background transparent
    TEXT_TRANSPARENCY_BACKGROUND_LIGHT, // Background slightly visible (25%)
    TEXT_TRANSPARENCY_BACKGROUND_MEDIUM,// Background partially visible (50%)
    TEXT_TRANSPARENCY_BACKGROUND_HEAVY  // Background mostly visible (75%)
} text_transparency_t;

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

/**
 * @brief Print multi-line text with word wrapping
 * 
 * Renders text with word wrapping at the specified coordinates.
 * 
 * @param y The Y coordinate (row)
 * @param x The X coordinate (column)
 * @param text The text to render
 * @param max_width The maximum width for line wrapping
 * @param ncchannel The color channel (foreground and background)
 */
void print_text_multi_line(int y, int x, const char* text, int max_width, u_int64_t ncchannel);

/**
 * @brief Print multi-line text with word wrapping and default colors
 * 
 * Renders text with word wrapping at the specified coordinates using default colors.
 * 
 * @param y The Y coordinate (row)
 * @param x The X coordinate (column)
 * @param text The text to render
 * @param max_width The maximum width for line wrapping
 */
void print_text_multi_line_default(int y, int x, const char* text, int max_width);

/**
 * @brief Print multiple strings on consecutive lines
 * 
 * Renders an array of strings starting at the specified coordinates.
 * 
 * @param y The Y coordinate (row) for the first string
 * @param x The X coordinate (column)
 * @param text Array of strings to render
 * @param count Number of strings in the array
 * @param ncchannel The color channel (foreground and background)
 */
void print_text_multi_strings(int y, int x, const char* text[], int count, uint64_t ncchannel);

/**
 * @brief Print multiple strings on consecutive lines with default colors
 * 
 * Renders an array of strings starting at the specified coordinates using default colors.
 * 
 * @param y The Y coordinate (row) for the first string
 * @param x The X coordinate (column)
 * @param text Array of strings to render
 * @param count Number of strings in the array
 */
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

/**
 * @brief Print a menu with selection highlighting using default colors
 * 
 * Renders a menu with the specified options, highlighting the selected option.
 * Uses default colors for normal options and inverted colors for the selected option.
 * 
 * @param title The menu title
 * @param options Array of option strings
 * @param option_count Number of options
 * @param selected_index Index of the selected option
 * @param y The Y coordinate (row) for the menu start
 * @param x The X coordinate (column) for the menu start
 */
void print_menu_default(const char* title, const char** options, int option_count,
                        int selected_index, int y, int x);

/**
 * @brief Get a text input from the user
 * 
 * Displays a prompt and gets a text input from the user.
 * 
 * @param prompt The text prompt to display
 * @param buffer The buffer to store the input in
 * @param buffer_size The size of the buffer
 * @param confirm_msg The text explaining that Enter confirms (can be NULL)
 * @param y The Y coordinate (row) for the prompt
 * @param x The X coordinate (column) for the prompt
 * @return true if input was confirmed, false if canceled
 */
bool get_text_input(const char* prompt, char* buffer, int buffer_size,
                    const char* confirm_msg, int y, int x);

/**
 * @brief Show a message screen
 * 
 * Displays a message on the screen and waits for user input.
 * 
 * @param message The message to display
 * @param continue_message Optional message explaining how to continue (can be NULL)
 * @param y The Y coordinate (row) for the message
 * @param x The X coordinate (column) for the message
 */
void show_message_screen(const char* message, const char* continue_message, int y, int x);

/**
 * @brief Print text with advanced formatting options
 * 
 * Renders text with alignment, style, transparency, and color options.
 * 
 * @param y The Y coordinate (row)
 * @param x The X coordinate (column)
 * @param text The text to render
 * @param ncchannel The color channel (foreground and background)
 * @param alignment Text alignment option
 * @param style Text style option
 * @param transparency Text transparency option
 * @param width Width for alignment calculation (0 for auto)
 */
void print_text_formatted(int y, int x, const char* text, uint64_t ncchannel,
                          text_align_t alignment, text_style_t style,
                          text_transparency_t transparency, int width);

/**
 * @brief Print text with alignment only
 * 
 * Renders text with specified alignment using default colors and style.
 * 
 * @param y The Y coordinate (row)
 * @param x The X coordinate (column)
 * @param text The text to render
 * @param alignment Text alignment option
 * @param width Width for alignment calculation
 */
void print_text_aligned(int y, int x, const char* text, text_align_t alignment, int width);

/**
 * @brief Print text with style only
 * 
 * Renders text with specified style using default colors and alignment.
 * 
 * @param y The Y coordinate (row)
 * @param x The X coordinate (column)
 * @param text The text to render
 * @param style Text style option
 * @param ncchannel The color channel (foreground and background)
 */
void print_text_styled(int y, int x, const char* text, text_style_t style, uint64_t ncchannel);

/**
 * @brief Print text with transparency only
 * 
 * Renders text with specified transparency using default colors and style.
 * 
 * @param y The Y coordinate (row)
 * @param x The X coordinate (column)
 * @param text The text to render
 * @param transparency Text transparency option
 * @param ncchannel The color channel (foreground and background)
 */
void print_text_transparent(int y, int x, const char* text, text_transparency_t transparency, uint64_t ncchannel);

#endif // TEXT_OUTPUT_H