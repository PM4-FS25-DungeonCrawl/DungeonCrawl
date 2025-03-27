#ifndef LOGGER_H
#define LOGGER_H

#define MAX_N_FILES 5
#define MAX_FILE_SIZE 1048576 // max size = 1 MB

// relative directory from main.c
#define LOG_DIRECTORY "../log"
#define LOG_FILE_FORMAT "log-%d.txt"

#define TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"
#define MSG_FORMAT "[%s] [%s] [%s] : %s\n"

#define MAX_HEADER_SIZE 256

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
