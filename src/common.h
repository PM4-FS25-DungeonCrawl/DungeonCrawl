#ifndef COMMON_H
#define COMMON_H

#include "logging/logger.h"
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
typedef struct string_max_t{
    char characters[MAX_STRING_LENGTH];
} string_max_t;

/**
 * The main memory pool for the game, used for dynamic memory allocation.
 */
extern memory_pool_t* main_memory_pool;

#endif//COMMON_H
