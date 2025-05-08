#include "pause_menu.h"
#include "../output/colors.h"
#include "../output/common/common_output.h"
#include "../../logging/logger.h"
#include "../../local/local.h" // For localization

pause_menu_result_t show_pause_menu(void) {
    // Get screen dimensions for centering
    int screen_width, screen_height;
    if (!get_screen_dimensions(&screen_width, &screen_height)) {
        log_msg(ERROR, "pause_menu", "Failed to get screen dimensions");
        return PAUSE_MENU_RESULT_RESUME; // Default to resume on error
    }
    
    // Create menu options
    // In a real implementation, we would use localized strings
    const char* options[] = {
        "Resume Game",
        "Save Game",
        "Load Game",
        "Options",
        "Return to Main Menu",
        "Quit Game"
    };
    
    int option_count = sizeof(options) / sizeof(options[0]);
    
    // Calculate menu position (centered)
    int menu_width = 20; // Approximate width of the menu
    int menu_x = (screen_width - menu_width) / 2;
    int menu_y = (screen_height - option_count - 1) / 2; // -1 for title
    
    // Create the menu
    menu_t* menu = menu_create("GAME PAUSED", options, option_count, menu_x, menu_y);
    if (!menu) {
        log_msg(ERROR, "pause_menu", "Failed to create pause menu");
        return PAUSE_MENU_RESULT_RESUME; // Default to resume on error
    }
    
    // Customize colors for the pause menu (darker background)
    uint32_t pause_bg = COLOR_BLACK; // Darker background for pause menu
    
    menu_set_colors(menu,
                   COLOR_UI_HIGHLIGHT, pause_bg,
                   COLOR_UI_FG, pause_bg,
                   COLOR_UI_HIGHLIGHT, pause_bg,
                   COLOR_WALL, pause_bg);
    
    // Draw a box around the menu for better visibility
    draw_box(menu_y - 1, menu_x - 2, option_count + 3, menu_width + 4, NULL, COLOR_UI_FG, pause_bg);
    
    // Run the menu
    int selected_index;
    menu_result_t result = menu_run(menu, &selected_index, NULL);
    
    // Determine the result
    pause_menu_result_t pause_menu_result;
    
    switch (result) {
        case MENU_RESULT_SELECTED:
            // Map the selected index to the corresponding result
            switch (selected_index) {
                case 0: pause_menu_result = PAUSE_MENU_RESULT_RESUME; break;
                case 1: pause_menu_result = PAUSE_MENU_RESULT_SAVE; break;
                case 2: pause_menu_result = PAUSE_MENU_RESULT_LOAD; break;
                case 3: pause_menu_result = PAUSE_MENU_RESULT_OPTIONS; break;
                case 4: pause_menu_result = PAUSE_MENU_RESULT_MAIN_MENU; break;
                case 5: pause_menu_result = PAUSE_MENU_RESULT_QUIT; break;
                default: pause_menu_result = PAUSE_MENU_RESULT_RESUME; break;
            }
            break;
            
        case MENU_RESULT_CANCELED:
            // ESC key should resume the game
            pause_menu_result = PAUSE_MENU_RESULT_RESUME;
            break;
            
        case MENU_RESULT_EXIT:
            pause_menu_result = PAUSE_MENU_RESULT_QUIT;
            break;
            
        case MENU_RESULT_ERROR:
        default:
            log_msg(ERROR, "pause_menu", "Error or unexpected result from menu_run");
            pause_menu_result = PAUSE_MENU_RESULT_RESUME;
            break;
    }
    
    // Free the menu
    menu_free(menu);
    
    return pause_menu_result;
}