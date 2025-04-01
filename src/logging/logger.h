#ifndef LOGGER_H
#define LOGGER_H

typedef enum {
    DEBUG,
    FINE,
    INFO,
    WARNING,
    ERROR
} LogLevel;

void close_log_file(int terminate_thread);

void log_msg(LogLevel level, const char *module, const char *format, ...);

#endif //LOGGER_H
