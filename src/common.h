#ifndef COMMON_H
#define COMMON_H

#include "logging/logger.h"

#include <notcurses/notcurses.h>

// defining notcurses channels
#define _BASE 40, 29, 16// 0x281D10
#define _FLOOR 50, 24, 14
// Colors for text and shit
#define DEFAULT_COLORS NCCHANNELS_INITIALIZER(235, 217, 167, 40, 29, 16)
#define INVERTED_COLORS NCCHANNELS_INITIALIZER(40, 29, 16, 235, 217, 167)
#define RED_TEXT_COLORS NCCHANNELS_INITIALIZER(142, 40, 24, 40, 29, 16)

// Colors for drawing the map
#define WALL_COLORS NCCHANNELS_INITIALIZER(156, 101, 73, 111, 56, 38)
#define EXIT_DOOR_COLORS NCCHANNELS_INITIALIZER(229, 175, 71, 50, 24, 14)
#define KEY_COLORS NCCHANNELS_INITIALIZER(229, 175, 71, 50, 24, 14)
#define GOBLIN_COLORS NCCHANNELS_INITIALIZER(142, 40, 24, 50, 24, 14)
#define HIDDEN_COLORS NCCHANNELS_INITIALIZER(142, 127, 109, 142, 127, 109)
#define START_DOOR_COLORS NCCHANNELS_INITIALIZER(64, 86, 37, 50, 24, 14)
#define FLOOR_COLORS NCCHANNELS_INITIALIZER(50, 24, 14, 50, 24, 14)

#define PLANE_COLOR 0, 0, 0
#include "memory/memory_management.h"

#define MAX_STRING_LENGTH 256
#define MAX_NAME_LENGTH 64

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
 * The main memory pool for the game, used for dynamic memory allocation.
 */
extern memory_pool_t* main_memory_pool;

#endif//COMMON_H
