#ifndef IO_DIALOG_H
#define IO_DIALOG_H

#include <stdbool.h>

/**
 * @brief Dialog result codes
 */
typedef enum {
    DIALOG_RESULT_YES = 0,   // User selected 'Yes'/'OK'
    DIALOG_RESULT_NO,        // User selected 'No'
    DIALOG_RESULT_CANCEL     // User canceled the dialog
} dialog_result_t;

/**
 * @brief Dialog button set options
 */
typedef enum {
    DIALOG_BUTTONS_OK,             // Single 'OK' button
    DIALOG_BUTTONS_OK_CANCEL,      // 'OK' and 'Cancel' buttons
    DIALOG_BUTTONS_YES_NO,         // 'Yes' and 'No' buttons
    DIALOG_BUTTONS_YES_NO_CANCEL   // 'Yes', 'No', and 'Cancel' buttons
} dialog_buttons_t;

/**
 * @brief Show a message dialog with specified buttons
 * 
 * @param title The dialog title
 * @param message The message to display
 * @param buttons The button set to use
 * @return The result of the dialog
 */
dialog_result_t show_dialog(const char* title, const char* message, dialog_buttons_t buttons);

/**
 * @brief Show a confirmation dialog (Yes/No)
 * 
 * @param title The dialog title
 * @param message The message to display
 * @return true if user selects 'Yes', false otherwise
 */
bool show_confirmation(const char* title, const char* message);

/**
 * @brief Show an information dialog (OK)
 * 
 * @param title The dialog title
 * @param message The message to display
 */
void show_message(const char* title, const char* message);

#endif // IO_DIALOG_H