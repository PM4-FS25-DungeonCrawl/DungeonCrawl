#include "dialog.h"
#include "menu.h"
#include "../output/colors.h"
#include "../output/common/common_output.h"
#include "../../logging/logger.h"
#include "../../local/local.h" // For localization

#include <string.h>

// Helper function to get button labels based on button set
static void get_button_labels(dialog_buttons_t buttons, const char*** labels, int* count) {
    static const char* ok_labels[] = { "OK" };
    static const char* ok_cancel_labels[] = { "OK", "Cancel" };
    static const char* yes_no_labels[] = { "Yes", "No" };
    static const char* yes_no_cancel_labels[] = { "Yes", "No", "Cancel" };
    
    switch (buttons) {
        case DIALOG_BUTTONS_OK:
            *labels = ok_labels;
            *count = 1;
            break;
            
        case DIALOG_BUTTONS_OK_CANCEL:
            *labels = ok_cancel_labels;
            *count = 2;
            break;
            
        case DIALOG_BUTTONS_YES_NO:
            *labels = yes_no_labels;
            *count = 2;
            break;
            
        case DIALOG_BUTTONS_YES_NO_CANCEL:
            *labels = yes_no_cancel_labels;
            *count = 3;
            break;
            
        default:
            *labels = ok_labels;
            *count = 1;
            break;
    }
}

// Helper function to create a dialog box with message and buttons
static menu_t* create_dialog(const char* title, const char* message, dialog_buttons_t buttons,
                           int* box_width, int* box_height) {
    // Get screen dimensions for centering
    int screen_width, screen_height;
    if (!get_screen_dimensions(&screen_width, &screen_height)) {
        log_msg(ERROR, "dialog", "Failed to get screen dimensions");
        return NULL;
    }
    
    // Get button labels
    const char** button_labels;
    int button_count;
    get_button_labels(buttons, &button_labels, &button_count);
    
    // Calculate dimensions for the dialog box
    int message_len = strlen(message);
    *box_width = message_len > 40 ? 50 : (message_len + 10);
    
    // Ensure minimum width for buttons
    int min_button_width = button_count * 10; // Approximate width for buttons
    if (*box_width < min_button_width) {
        *box_width = min_button_width;
    }
    
    *box_height = 6; // Title, message, spacing, buttons
    
    // Calculate dialog position (centered)
    int dialog_x = (screen_width - *box_width) / 2;
    int dialog_y = (screen_height - *box_height) / 2;
    
    // Calculate button position
    int buttons_y = dialog_y + 3;
    int buttons_x = dialog_x + (*box_width - (button_count * 8)) / 2; // Approximate centering of buttons
    
    // Create the menu for buttons
    menu_t* menu = menu_create("", button_labels, button_count, buttons_x, buttons_y);
    if (!menu) {
        log_msg(ERROR, "dialog", "Failed to create dialog menu");
        return NULL;
    }
    
    // Set colors
    menu_set_colors(menu,
                   COLOR_UI_HIGHLIGHT, COLOR_UI_BG,
                   COLOR_UI_FG, COLOR_UI_BG,
                   COLOR_UI_HIGHLIGHT, COLOR_UI_BG,
                   COLOR_WALL, COLOR_UI_BG);
    
    return menu;
}

dialog_result_t show_dialog(const char* title, const char* message, dialog_buttons_t buttons) {
    if (!title || !message) {
        log_msg(ERROR, "dialog", "Null title or message in show_dialog");
        return DIALOG_RESULT_CANCEL;
    }
    
    // Calculate dialog dimensions and create the menu
    int box_width, box_height;
    menu_t* menu = create_dialog(title, message, buttons, &box_width, &box_height);
    if (!menu) {
        return DIALOG_RESULT_CANCEL;
    }
    
    // Get screen dimensions for centering
    int screen_width, screen_height;
    get_screen_dimensions(&screen_width, &screen_height);
    
    // Calculate dialog position (centered)
    int dialog_x = (screen_width - box_width) / 2;
    int dialog_y = (screen_height - box_height) / 2;
    
    // Draw the dialog box
    clear_screen(COLOR_BLACK); // Semi-transparent background
    draw_box(dialog_y, dialog_x, box_height, box_width, title, COLOR_UI_FG, COLOR_UI_BG);
    
    // Draw the message
    print_text(dialog_y + 2, dialog_x + 2, message, COLOR_UI_FG, COLOR_UI_BG);
    
    // Run the menu
    int selected_index;
    menu_result_t result = menu_run(menu, &selected_index, NULL);
    
    // Determine the dialog result
    dialog_result_t dialog_result;
    
    switch (result) {
        case MENU_RESULT_SELECTED:
            // Map the selected index to the corresponding result
            switch (buttons) {
                case DIALOG_BUTTONS_OK:
                    dialog_result = DIALOG_RESULT_YES; // OK maps to Yes
                    break;
                    
                case DIALOG_BUTTONS_OK_CANCEL:
                    dialog_result = (selected_index == 0) ? DIALOG_RESULT_YES : DIALOG_RESULT_CANCEL;
                    break;
                    
                case DIALOG_BUTTONS_YES_NO:
                    dialog_result = (selected_index == 0) ? DIALOG_RESULT_YES : DIALOG_RESULT_NO;
                    break;
                    
                case DIALOG_BUTTONS_YES_NO_CANCEL:
                    if (selected_index == 0) {
                        dialog_result = DIALOG_RESULT_YES;
                    } else if (selected_index == 1) {
                        dialog_result = DIALOG_RESULT_NO;
                    } else {
                        dialog_result = DIALOG_RESULT_CANCEL;
                    }
                    break;
                    
                default:
                    dialog_result = DIALOG_RESULT_CANCEL;
                    break;
            }
            break;
            
        case MENU_RESULT_CANCELED:
        case MENU_RESULT_EXIT:
        case MENU_RESULT_ERROR:
        default:
            dialog_result = DIALOG_RESULT_CANCEL;
            break;
    }
    
    // Free the menu
    menu_free(menu);
    
    return dialog_result;
}

bool show_confirmation(const char* title, const char* message) {
    dialog_result_t result = show_dialog(title, message, DIALOG_BUTTONS_YES_NO);
    return (result == DIALOG_RESULT_YES);
}

void show_message(const char* title, const char* message) {
    show_dialog(title, message, DIALOG_BUTTONS_OK);
}