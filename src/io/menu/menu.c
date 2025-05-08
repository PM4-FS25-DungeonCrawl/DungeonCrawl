#include "menu.h"
#include "../output/colors.h"
#include "../output/common/common_output.h"
#include "../input/input_handler.h"
#include "../../logging/logger.h"

#include <stdlib.h>
#include <string.h>

menu_t* menu_create(const char* title, const char** options, int option_count, int x, int y) {
    if (!title || !options || option_count <= 0) {
        log_msg(ERROR, "menu", "Invalid parameters for menu_create");
        return NULL;
    }
    
    // Allocate the menu structure
    menu_t* menu = malloc(sizeof(menu_t));
    if (!menu) {
        log_msg(ERROR, "menu", "Failed to allocate memory for menu");
        return NULL;
    }
    
    // Initialize the structure
    memset(menu, 0, sizeof(menu_t));
    
    // Copy the title
    menu->title = strdup(title);
    if (!menu->title) {
        log_msg(ERROR, "menu", "Failed to allocate memory for menu title");
        free(menu);
        return NULL;
    }
    
    // Allocate the options array
    menu->options = malloc(option_count * sizeof(menu_option_t));
    if (!menu->options) {
        log_msg(ERROR, "menu", "Failed to allocate memory for menu options");
        free(menu->title);
        free(menu);
        return NULL;
    }
    
    // Copy the options
    for (int i = 0; i < option_count; i++) {
        menu->options[i].text = strdup(options[i]);
        if (!menu->options[i].text) {
            log_msg(ERROR, "menu", "Failed to allocate memory for menu option %d", i);
            
            // Free previously allocated options
            for (int j = 0; j < i; j++) {
                free(menu->options[j].text);
            }
            
            free(menu->options);
            free(menu->title);
            free(menu);
            return NULL;
        }
        
        menu->options[i].user_data = NULL;
        menu->options[i].enabled = true;
    }
    
    // Set the remaining fields
    menu->option_count = option_count;
    menu->selected_index = 0;
    menu->x = x;
    menu->y = y;
    
    // Set default colors
    menu->title_fg = COLOR_UI_HIGHLIGHT;
    menu->title_bg = COLOR_UI_BG;
    menu->option_fg = COLOR_UI_FG;
    menu->option_bg = COLOR_UI_BG;
    menu->selected_fg = COLOR_UI_HIGHLIGHT;
    menu->selected_bg = COLOR_UI_BG;
    menu->disabled_fg = COLOR_WALL;
    menu->disabled_bg = COLOR_UI_BG;
    
    return menu;
}

void menu_set_colors(menu_t* menu, 
                    uint32_t title_fg, uint32_t title_bg,
                    uint32_t option_fg, uint32_t option_bg,
                    uint32_t selected_fg, uint32_t selected_bg,
                    uint32_t disabled_fg, uint32_t disabled_bg) {
    if (!menu) {
        log_msg(ERROR, "menu", "Null menu pointer in menu_set_colors");
        return;
    }
    
    menu->title_fg = title_fg;
    menu->title_bg = title_bg;
    menu->option_fg = option_fg;
    menu->option_bg = option_bg;
    menu->selected_fg = selected_fg;
    menu->selected_bg = selected_bg;
    menu->disabled_fg = disabled_fg;
    menu->disabled_bg = disabled_bg;
}

bool menu_set_option_enabled(menu_t* menu, int index, bool enabled) {
    if (!menu || index < 0 || index >= menu->option_count) {
        log_msg(ERROR, "menu", "Invalid parameters for menu_set_option_enabled");
        return false;
    }
    
    menu->options[index].enabled = enabled;
    return true;
}

bool menu_set_option_user_data(menu_t* menu, int index, void* user_data) {
    if (!menu || index < 0 || index >= menu->option_count) {
        log_msg(ERROR, "menu", "Invalid parameters for menu_set_option_user_data");
        return false;
    }
    
    menu->options[index].user_data = user_data;
    return true;
}

bool menu_render(const menu_t* menu) {
    if (!menu) {
        log_msg(ERROR, "menu", "Null menu pointer in menu_render");
        return false;
    }
    
    // Print the title
    print_text(menu->y, menu->x, menu->title, menu->title_fg, menu->title_bg);
    
    // Print the options
    for (int i = 0; i < menu->option_count; i++) {
        uint32_t fg, bg;
        
        if (!menu->options[i].enabled) {
            // Disabled option
            fg = menu->disabled_fg;
            bg = menu->disabled_bg;
        } else if (i == menu->selected_index) {
            // Selected option
            fg = menu->selected_fg;
            bg = menu->selected_bg;
        } else {
            // Normal option
            fg = menu->option_fg;
            bg = menu->option_bg;
        }
        
        print_text(menu->y + i + 1, menu->x, menu->options[i].text, fg, bg);
    }
    
    // Render the changes
    render_frame();
    
    return true;
}

menu_result_t menu_handle_input(menu_t* menu, const input_event_t* event) {
    if (!menu || !event) {
        log_msg(ERROR, "menu", "Invalid parameters for menu_handle_input");
        return MENU_RESULT_ERROR;
    }
    
    // Process the input
    switch (event->type) {
        case INPUT_UP:
            // Move selection up
            do {
                menu->selected_index = (menu->selected_index - 1 + menu->option_count) % menu->option_count;
            } while (!menu->options[menu->selected_index].enabled && menu->selected_index != 0);
            return MENU_RESULT_NONE;
            
        case INPUT_DOWN:
            // Move selection down
            do {
                menu->selected_index = (menu->selected_index + 1) % menu->option_count;
            } while (!menu->options[menu->selected_index].enabled && menu->selected_index != 0);
            return MENU_RESULT_NONE;
            
        case INPUT_CONFIRM:
            // Select the current option
            if (menu->options[menu->selected_index].enabled) {
                return MENU_RESULT_SELECTED;
            }
            return MENU_RESULT_NONE;
            
        case INPUT_CANCEL:
            // Cancel the menu
            return MENU_RESULT_CANCELED;
            
        case INPUT_QUIT:
            // Exit the program
            return MENU_RESULT_EXIT;
            
        default:
            // Ignore other inputs
            return MENU_RESULT_NONE;
    }
}

menu_result_t menu_run(menu_t* menu, int* selected_index, void** selected_data) {
    if (!menu) {
        log_msg(ERROR, "menu", "Null menu pointer in menu_run");
        return MENU_RESULT_ERROR;
    }
    
    // Render the initial menu
    menu_render(menu);
    
    // Main menu loop
    while (true) {
        // Get the next input event
        input_event_t event;
        if (!get_input_blocking(&event)) {
            // Error getting input
            log_msg(ERROR, "menu", "Failed to get input in menu_run");
            return MENU_RESULT_ERROR;
        }
        
        // Process the input
        menu_result_t result = menu_handle_input(menu, &event);
        
        // Check if we need to exit the loop
        if (result == MENU_RESULT_SELECTED || result == MENU_RESULT_CANCELED || result == MENU_RESULT_EXIT) {
            // Set the output parameters if needed
            if (result == MENU_RESULT_SELECTED) {
                if (selected_index) {
                    *selected_index = menu->selected_index;
                }
                if (selected_data) {
                    *selected_data = menu->options[menu->selected_index].user_data;
                }
            }
            
            return result;
        }
        
        // Re-render the menu if needed
        if (result == MENU_RESULT_NONE) {
            menu_render(menu);
        }
    }
}

void menu_free(menu_t* menu) {
    if (!menu) {
        return;
    }
    
    // Free the options
    if (menu->options) {
        for (int i = 0; i < menu->option_count; i++) {
            free(menu->options[i].text);
        }
        free(menu->options);
    }
    
    // Free the title
    free(menu->title);
    
    // Free the menu structure
    free(menu);
}