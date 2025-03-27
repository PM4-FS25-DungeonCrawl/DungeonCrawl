//
// Created by ninof on 27.03.2025.
//

#ifndef LOGGER_H
#define LOGGER_H

#define MAX_N_FILES 5
#define MAX_FILE_SIZE 1024 * 1024 // max siz = 1 MB

// relative directory
#define LOG_DIRECTORY "../../log/"
#define LOG_FILE_FORMAT "log-%d.txt"

#define TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"
#define MSG_FORMAT "[%s] [%s] [%s] : %s\n"

typedef enum {
    DEBUG,
    FINE,
    INFO,
    WARNING,
    ERROR
} LogLevel;

void close_log_file(void);

void log_msg(LogLevel level, const char *module, const char *format, ...);

#endif //LOGGER_H
