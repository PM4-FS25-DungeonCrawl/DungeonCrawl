#ifndef LOGGER_H
#define LOGGER_H

typedef enum {
    DEBUG,
    FINE,
    INFO,
    WARNING,
    ERROR,
    MAX_LOG_LEVEL
} log_level_t;

void init_logger(void);
void log_msg(log_level_t level, const char* module, const char* format, ...);
void shutdown_logger(void);
#endif//LOGGER_H
