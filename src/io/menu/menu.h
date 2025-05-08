#ifndef IO_MENU_H
#define IO_MENU_H

#include "../input/input_types.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Menu option structure
 */
typedef struct menu_option_s {
    char* text;              // Display text for the option
    void* user_data;         // Optional user-provided data
    bool enabled;            // Whether the option can be selected
} menu_option_t;

/**
 * @brief Menu interaction result codes
 */
typedef enum {
    MENU_RESULT_NONE,        // No result yet
    MENU_RESULT_SELECTED,    // An option was selected (ENTER)
    MENU_RESULT_CANCELED,    // Menu was canceled (ESC)
    MENU_RESULT_EXIT,        // Special exit (custom handling)
    MENU_RESULT_ERROR        // An error occurred
} menu_result_t;

/**
 * @brief Menu structure
 */
typedef struct menu_s {
    char* title;                     // Menu title
    menu_option_t* options;          // Array of menu options
    int option_count;                // Number of options
    int selected_index;              // Currently selected option index
    int x;                           // X position for rendering
    int y;                           // Y position for rendering
    uint32_t title_fg;               // Title foreground color
    uint32_t title_bg;               // Title background color
    uint32_t option_fg;              // Option foreground color
    uint32_t option_bg;              // Option background color
    uint32_t selected_fg;            // Selected option foreground color
    uint32_t selected_bg;            // Selected option background color
    uint32_t disabled_fg;            // Disabled option foreground color
    uint32_t disabled_bg;            // Disabled option background color
} menu_t;

/**
 * @brief Initialize a new menu
 * 
 * @param title The menu title
 * @param options Array of option strings (will be copied)
 * @param option_count Number of options
 * @param x X position for rendering
 * @param y Y position for rendering
 * @return Pointer to the new menu, or NULL on failure
 */
menu_t* menu_create(const char* title, const char** options, int option_count, int x, int y);

/**
 * @brief Set custom colors for a menu
 * 
 * @param menu The menu to modify
 * @param title_fg Title foreground color
 * @param title_bg Title background color
 * @param option_fg Option foreground color
 * @param option_bg Option background color
 * @param selected_fg Selected option foreground color
 * @param selected_bg Selected option background color
 * @param disabled_fg Disabled option foreground color
 * @param disabled_bg Disabled option background color
 */
void menu_set_colors(menu_t* menu, 
                    uint32_t title_fg, uint32_t title_bg,
                    uint32_t option_fg, uint32_t option_bg,
                    uint32_t selected_fg, uint32_t selected_bg,
                    uint32_t disabled_fg, uint32_t disabled_bg);

/**
 * @brief Set the enabled state of a menu option
 * 
 * @param menu The menu to modify
 * @param index The option index to modify
 * @param enabled Whether the option should be enabled
 * @return true on success, false on failure
 */
bool menu_set_option_enabled(menu_t* menu, int index, bool enabled);

/**
 * @brief Set user data for a menu option
 * 
 * @param menu The menu to modify
 * @param index The option index to modify
 * @param user_data The user data to associate with the option
 * @return true on success, false on failure
 */
bool menu_set_option_user_data(menu_t* menu, int index, void* user_data);

/**
 * @brief Render a menu
 * 
 * @param menu The menu to render
 * @return true on success, false on failure
 */
bool menu_render(const menu_t* menu);

/**
 * @brief Handle input for a menu
 * 
 * @param menu The menu to update
 * @param event The input event to process
 * @return The menu result (selected, canceled, etc.)
 */
menu_result_t menu_handle_input(menu_t* menu, const input_event_t* event);

/**
 * @brief Run a menu loop with blocking input
 * 
 * This function handles both input processing and rendering in a loop until
 * the user makes a selection or cancels the menu.
 * 
 * @param menu The menu to run
 * @param selected_index Pointer to store the selected index (if MENU_RESULT_SELECTED)
 * @param selected_data Pointer to store the selected option's user data (if MENU_RESULT_SELECTED)
 * @return The menu result (selected, canceled, etc.)
 */
menu_result_t menu_run(menu_t* menu, int* selected_index, void** selected_data);

/**
 * @brief Free a menu and all associated resources
 * 
 * @param menu The menu to free
 */
void menu_free(menu_t* menu);

#endif // IO_MENU_H