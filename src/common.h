#ifndef COMMON_H
#define COMMON_H

#include <src/logging/logger.h>

#define MAX_STRING_LENGTH 256
#define MAX_NAME_LENGTH 64


#define NULL_PTR_HANDLER_RETURN(ptr, return_value, modul, format, ...)     \
    if (ptr == NULL) {                                \
        log_msg(ERROR, modul, format, ##__VA_ARGS__); \
        return return_value;                          \
    }

/**
 * @brief 2-dimensional vector struct
 */
typedef struct vector2d_t{
    int dx;
    int dy;
} vector2d_t;

#endif//COMMON_H
