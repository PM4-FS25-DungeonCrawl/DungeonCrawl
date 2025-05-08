#ifndef COLORS_H
#define COLORS_H

#include <stdint.h>

// Common colors as RGB values
typedef enum {
    // Basic colors
    COLOR_BLACK =       0x000000,
    COLOR_RED =         0xFF0000,
    COLOR_GREEN =       0x00FF00,
    COLOR_YELLOW =      0xFFFF00,
    COLOR_BLUE =        0x0000FF,
    COLOR_MAGENTA =     0xFF00FF,
    COLOR_CYAN =        0x00FFFF,
    COLOR_WHITE =       0xFFFFFF,
    
    // UI colors
    COLOR_UI_BG =       0x281D10, // Dark brown background
    COLOR_UI_FG =       0xE0D0B0, // Light tan foreground
    COLOR_UI_HIGHLIGHT = 0xFFD700, // Gold for highlighting
    COLOR_UI_DANGER =   0xD72305, // Dark red for danger/warnings
    
    // Game specific colors
    COLOR_PLAYER =      0x00FF00, // Green for player
    COLOR_MONSTER =     0xFF0000, // Red for monsters
    COLOR_ITEM =        0xFFFF00, // Yellow for items
    COLOR_FLOOR =       0x808080, // Gray for floor
    COLOR_WALL =        0x383838, // Dark gray for walls
    COLOR_DOOR =        0x8B4513, // Brown for doors
    
    // Default colors (for text)
    COLOR_DEFAULT_FG =  0xFFFFFF, // White
    COLOR_DEFAULT_BG =  0x000000  // Black
} color_t;

// Helper macros to extract RGB components
#define COLOR_R(color) (((color) >> 16) & 0xFF)
#define COLOR_G(color) (((color) >> 8) & 0xFF)
#define COLOR_B(color) ((color) & 0xFF)

// Helper to create an RGB color from components
#define RGB(r, g, b) ((((uint32_t)(r)) << 16) | (((uint32_t)(g)) << 8) | ((uint32_t)(b)))

// Helper to create a channels value for Notcurses
// Combines a foreground and background color into a Notcurses channels value
uint64_t make_channels(uint32_t fg, uint32_t bg);

#endif // COLORS_H