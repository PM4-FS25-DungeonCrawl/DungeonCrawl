/**
 * @file common.h
 * @brief Defines common macros, types, and global variables for color schemes and utilities.
 */
#ifndef COMMON_H
#define COMMON_H

#include "logging/logger.h"

// change this to select color scheme
#define PYXEL
// I know this looks dumb af but this is the easiest way I have found
#ifdef PYXEL
    // AAP12 Palette
    // define Text color
    #define FG_R 253
    #define FG_G 253
    #define FG_B 253
    // define background color
    #define BG_R 0
    #define BG_G 0
    #define BG_B 0
    // define grey
    #define GREY_R 144
    #define GREY_G 144
    #define GREY_B 144
    // define red
    #define RED_R 180
    #define RED_G 33
    #define RED_B 44
    // define yellow
    #define YELLOW_R 244
    #define YELLOW_G 220
    #define YELLOW_B 95
    // define green
    #define GREEN_R 151
    #define GREEN_G 197
    #define GREEN_B 34
    // define blue
    #define BLUE_R 43
    #define BLUE_G 152
    #define BLUE_B 240
    // define brown
    #define BROWN_R 152
    #define BROWN_G 88
    #define BROWN_B 30
#endif// PYXEL
#ifdef AAP12
    // AAP12 Palette
    // define Text color
    #define FG_R 248
    #define FG_G 244
    #define FG_B 236
    // define background color
    #define BG_R 5
    #define BG_G 4
    #define BG_B 4
    // define grey
    #define GREY_R 26
    #define GREY_G 22
    #define GREY_B 23
    // define red
    #define RED_R 151
    #define RED_G 53
    #define RED_B 52
    // define yellow
    #define YELLOW_R 238
    #define YELLOW_G 206
    #define YELLOW_B 103
    // define green
    #define GREEN_R 97
    #define GREEN_G 129
    #define GREEN_B 70
    // define blue
    #define BLUE_R 62
    #define BLUE_G 57
    #define BLUE_B 96
    // define brown
    #define BROWN_R 69
    #define BROWN_G 36
    #define BROWN_B 41
#endif// AAP12
#ifdef PICO8
    // Pico8 Palette
    // define Text color
    #define FG_R 255
    #define FG_G 238
    #define FG_B 227
    // define background color
    #define BG_R 0
    #define BG_G 0
    #define BG_B 0
    // define grey
    #define GREY_R 184
    #define GREY_G 185
    #define GREY_B 189
    // define red
    #define RED_R 255
    #define RED_G 9
    #define RED_B 69
    // define yellow
    #define YELLOW_R 254
    #define YELLOW_G 232
    #define YELLOW_B 39
    // define green
    #define GREEN_R 0
    #define GREEN_G 223
    #define GREEN_B 50
    // define blue
    #define BLUE_R 39
    #define BLUE_G 161
    #define BLUE_B 254
    // define brown
    #define BROWN_R 159
    #define BROWN_G 72
    #define BROWN_B 49
#endif// PICO8

// defining notcurses channels
#define DEFAULT_COLORS NCCHANNELS_INITIALIZER(FG_R, FG_G, FG_B, BG_R, BG_G, BG_B)
#define INVERTED_COLORS NCCHANNELS_INITIALIZER(BG_R, BG_G, BG_B, FG_R, FG_G, FG_B)
#define RED_TEXT_COLORS NCCHANNELS_INITIALIZER(RED_R, RED_G, RED_B, BG_R, BG_G, BG_B)

// Colors for drawing the map
#define WALL_COLORS NCCHANNELS_INITIALIZER(BLUE_R, BLUE_G, BLUE_B, BROWN_R, BROWN_G, BROWN_B)
#define EXIT_DOOR_COLORS NCCHANNELS_INITIALIZER(YELLOW_R, YELLOW_G, YELLOW_B, BG_R, BG_G, BG_B)
#define KEY_COLORS NCCHANNELS_INITIALIZER(YELLOW_R, YELLOW_G, YELLOW_B, BG_R, BG_G, BG_B)
#define GOBLIN_COLORS NCCHANNELS_INITIALIZER(RED_R, RED_G, RED_B, BG_R, BG_G, BG_B)
#define HIDDEN_COLORS NCCHANNELS_INITIALIZER(GREY_R, GREY_G, GREY_B, GREY_R, GREY_G, GREY_B)
#define START_DOOR_COLORS NCCHANNELS_INITIALIZER(GREEN_R, GREEN_G, GREEN_B, BG_R, BG_G, BG_B)
#define FLOOR_COLORS NCCHANNELS_INITIALIZER(BG_R, BG_G, BG_B, BG_R, BG_G, BG_B)
#define LIFE_FOUNTAIN_COLORS NCCHANNELS_INITIALIZER(BG_R, BG_G, BG_B, GREEN_R, GREEN_G, GREEN_B)
#define MANA_FOUNTAIN_COLORS NCCHANNELS_INITIALIZER(BG_R, BG_G, BG_B, BLUE_R, BLUE_G, BLUE_B)

#include "memory/memory_management.h"

#define MAX_STRING_LENGTH 256
#define MAX_NAME_LENGTH 64

/**
 * @brief Common success return value.
 *
 * This constant is used to indicate a successful operation in various functions
 * throughout the application.
 */
#define COMMON_SUCCESS 0


#define NULL_PTR_HANDLER_RETURN(ptr, return_value, modul, format, ...) \
    if (ptr == NULL) {                                                 \
        log_msg(ERROR, modul, format, ##__VA_ARGS__);                  \
        return return_value;                                           \
    }

#define CHECK_ARG_RETURN(bool_expr, return_value, modul, format, ...) \
    if (bool_expr) {                                                  \
        log_msg(ERROR, modul, format, ##__VA_ARGS__);                 \
        return return_value;                                          \
    }

/**
 * @brief 2-dimensional vector struct
 */
typedef struct vector2d_t {
    int dx;
    int dy;
} vector2d_t;

/**
 * @brief String struct with a fixed maximum length, it is directly linked to the macro MAX_STRING_LENGTH
 */
typedef struct string_max_t {
    char characters[MAX_STRING_LENGTH];
} string_max_t;

/**
 * @brief Global memory pool for the application.
 *
 * This variable holds the main memory pool used for dynamic memory allocation
 * throughout the application. It is initialized during the application's startup
 * and should be properly managed to avoid memory leaks.
 */
extern memory_pool_t* main_memory_pool;

#endif//COMMON_H
