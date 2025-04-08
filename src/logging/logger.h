#ifndef LOGGER_H
#define LOGGER_H

typedef enum {
    DEBUG,
    FINE,
    INFO,
    WARNING,
    ERROR
} log_level_t;

void close_log_file(bool terminate_thread);

void log_msg(log_level_t level, const char* module, const char* format, ...);

#endif//LOGGER_H
