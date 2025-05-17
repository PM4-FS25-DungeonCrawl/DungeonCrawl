#ifndef INPUT_TYPES_H
#define INPUT_TYPES_H

#include <notcurses/notcurses.h>

/**
 * @brief Enumeration of all possible input actions in the game
 * 
 * This enum maps hardware-specific events to logical game actions,
 * providing an abstraction layer between the input hardware and
 * the game logic.
 */
typedef enum {
    // Navigation inputs
    INPUT_UP,
    INPUT_DOWN,
    INPUT_LEFT,
    INPUT_RIGHT,

    // Action inputs
    INPUT_CONFIRM,  // Enter/Space/Primary action
    INPUT_CANCEL,   // Escape/Secondary action
    INPUT_MENU,     // Open menu (M key)
    INPUT_STATS,    // Show stats (L key)
    INPUT_INVENTORY,// Show inventory (I key)

    // Special inputs
    INPUT_QUIT,// Ctrl+C or other quit command

    // No input received
    INPUT_NONE
} input_t;

/**
 * @brief Structure for advanced input events with context data
 * 
 * This structure combines our logical input type with the raw Notcurses input data.
 */
typedef struct {
    input_t type;     // The logical input type
    ncinput raw_input;// The raw Notcurses input data
} input_event_t;

#endif// INPUT_TYPES_H