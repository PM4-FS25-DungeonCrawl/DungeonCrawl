#include "main_menu.h"
#include "../output/colors.h"
#include "../output/common/common_output.h"
#include "../../logging/logger.h"
#include "../../local/local.h" // For localization

main_menu_result_t show_main_menu(void) {
    // Get screen dimensions for centering
    int screen_width, screen_height;
    if (!get_screen_dimensions(&screen_width, &screen_height)) {
        log_msg(ERROR, "main_menu", "Failed to get screen dimensions");
        return MAIN_MENU_RESULT_QUIT; // Default to quit on error
    }
    
    // Create menu options
    // In a real implementation, we would use localized strings
    const char* options[] = {
        "Continue Game",
        "New Game",
        "Load Game",
        "Options",
        "Credits",
        "Quit"
    };
    
    int option_count = sizeof(options) / sizeof(options[0]);
    
    // Calculate menu position (centered)
    int menu_width = 15; // Approximate width of the menu
    int menu_x = (screen_width - menu_width) / 2;
    int menu_y = (screen_height - option_count - 1) / 2; // -1 for title
    
    // Create the menu
    menu_t* menu = menu_create("DUNGEON CRAWL", options, option_count, menu_x, menu_y);
    if (!menu) {
        log_msg(ERROR, "main_menu", "Failed to create main menu");
        return MAIN_MENU_RESULT_QUIT; // Default to quit on error
    }
    
    // Customize colors if desired
    menu_set_colors(menu,
                   COLOR_UI_HIGHLIGHT, COLOR_UI_BG,
                   COLOR_UI_FG, COLOR_UI_BG,
                   COLOR_UI_HIGHLIGHT, COLOR_UI_BG,
                   COLOR_WALL, COLOR_UI_BG);
    
    // Disable "Continue Game" if no game in progress
    // This would need to be determined based on game state
    bool game_in_progress = false; // Example
    menu_set_option_enabled(menu, 0, game_in_progress);
    
    // Clear the screen
    clear_screen(COLOR_UI_BG);
    
    // Run the menu
    int selected_index;
    menu_result_t result = menu_run(menu, &selected_index, NULL);
    
    // Determine the result
    main_menu_result_t main_menu_result;
    
    switch (result) {
        case MENU_RESULT_SELECTED:
            // Map the selected index to the corresponding result
            switch (selected_index) {
                case 0: main_menu_result = MAIN_MENU_RESULT_CONTINUE; break;
                case 1: main_menu_result = MAIN_MENU_RESULT_NEW_GAME; break;
                case 2: main_menu_result = MAIN_MENU_RESULT_LOAD_GAME; break;
                case 3: main_menu_result = MAIN_MENU_RESULT_OPTIONS; break;
                case 4: main_menu_result = MAIN_MENU_RESULT_CREDITS; break;
                case 5: main_menu_result = MAIN_MENU_RESULT_QUIT; break;
                default: main_menu_result = MAIN_MENU_RESULT_QUIT; break;
            }
            break;
            
        case MENU_RESULT_CANCELED:
        case MENU_RESULT_EXIT:
            main_menu_result = MAIN_MENU_RESULT_QUIT;
            break;
            
        case MENU_RESULT_ERROR:
        default:
            log_msg(ERROR, "main_menu", "Error or unexpected result from menu_run");
            main_menu_result = MAIN_MENU_RESULT_QUIT;
            break;
    }
    
    // Free the menu
    menu_free(menu);
    
    return main_menu_result;
}