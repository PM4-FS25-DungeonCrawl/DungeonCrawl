#ifndef COMMON_H
#define COMMON_H

#include "logging/logger.h"

#define MAX_NAME_LENGTH 64

#define NULL_PTR_HANDLER(ptr, modul, format, ...) \
    if (ptr == NULL) { \
        log_msg(ERROR, modul, format, ##__VA_ARGS__); \
        return NULL; \
    }

#endif //COMMON_H
