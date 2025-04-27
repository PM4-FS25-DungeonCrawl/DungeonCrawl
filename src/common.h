#ifndef COMMON_H
#define COMMON_H

#include "logging/logger.h"
#include <notcurses/notcurses.h>

// defining notcurses channels
#define RED_ON_BLACK NCCHANNELS_INITIALIZER(255, 0, 0, 0, 0, 0)
#define WHITE_ON_BLACK NCCHANNELS_INITIALIZER(255, 255, 255, 0, 0, 0)
#define BLACK_ON_WHITE NCCHANNELS_INITIALIZER(0, 0, 0, 255, 255, 255)

#define MAX_STRING_LENGTH 256
#define MAX_NAME_LENGTH 64


#define NULL_PTR_HANDLER_RETURN(ptr, return_value, modul, format, ...) \
    if (ptr == NULL) {                                                 \
        log_msg(ERROR, modul, format, ##__VA_ARGS__);                  \
        return return_value;                                           \
    }

#define NULL_PTR_HANDLER_VOID(ptr, modul, format, ...) \
    if (ptr == NULL) {                                 \
        log_msg(ERROR, modul, format, ##__VA_ARGS__);  \
        return;                                        \
    }

#define CHECK_ARG_RETURN(bool_expr, return_value, modul, format, ...) \
    if (bool_expr) {                                                  \
        log_msg(ERROR, modul, format, ##__VA_ARGS__);                 \
        return return_value;                                          \
    }

#define CHECK_ARG_VOID(bool_expr, modul, format, ...) \
    if (bool_expr) {                                  \
        log_msg(ERROR, modul, format, ##__VA_ARGS__); \
        return;                                       \
    }

/**
 * @brief 2-dimensional vector struct
 */
typedef struct vector2d_t {
    int dx;
    int dy;
} vector2d_t;

#endif//COMMON_H
