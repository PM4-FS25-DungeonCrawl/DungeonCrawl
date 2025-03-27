//
// Created by ninof on 27.03.2025.
//

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>

#include "logger.h"

const char *log_level_str[] = {"DEBUG", "FINE", "INFO", "WARNING", "ERROR"};

char filename[256];
FILE *log_file = nullptr;

int file_id = 0;

void open_log_file(void) {
    //open file in append modus
    char name[15];
    snprintf(name, 15, LOG_FILE_FORMAT, file_id);

    snprintf(filename, 256, "%s/%s", LOG_DIRECTORY, name);

    log_file = fopen(filename, "a");
    if (!log_file) {
        //file could not be open
    }
}

/**
 * This function should be called, whenever a new log msg must be written.
 *
 * Check if the log file is open, if not a new file will be open.
 * - Either by creating a new file, if no file is in the log directory. Log file will get id = 0.
 * - Or creating a new file, if the last used file reached the max size.
 *   The new file will get the current file id + 1 or 0, if the id reached the max number of files.
 * - Or opening the file that was last used, the current id will be set.
 */
void check_log_file(void) {
    if (log_file) {
        // Überprüfen, ob die aktuelle Logdatei die maximale Größe erreicht hat
        fseek(log_file, 0, SEEK_END);
        const long file_size = ftell(log_file);
        if (file_size >= MAX_FILE_SIZE) {
            // the max size is reached
            close_log_file();
            file_id = (file_id + 1) % MAX_N_FILES;
            open_log_file();
        }
    }
}

/**
 * Gets from the file (last used) in the log directory, the id.
 * @return the latest used file id, in the range 0 (ink.) to MAX_N_FILES (exkl.) or -1 if dir could not be read.
 */
int get_latest_file_id(void) {
    DIR *dir = opendir(LOG_DIRECTORY);
    struct dirent *entry;
    int latest_id = 0;
    time_t latest_time = 0;

    if (!dir) {
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        int id;
        struct stat file_stat;

        if (sscanf(entry->d_name, LOG_FILE_FORMAT, &id) == 1 && id >= 0 && id < MAX_N_FILES) {
            char filepath[PATH_MAX];
            snprintf(filepath, sizeof(filepath), "%s/%s", LOG_DIRECTORY, entry->d_name);

            if (stat(filepath, &file_stat) == 0) {
                if (file_stat.st_mtime > latest_time) {
                    latest_time = file_stat.st_mtime;
                    latest_id = id;
                }
            }
        }
    }

    closedir(dir);

    return latest_id;
}

/**
 * Closes the current log file
 */
void close_log_file(void) {
    if (log_file) {
        fclose(log_file);
        log_file = nullptr;
    }
}

/**
 * Writes a log message to the log file.
 *
 * This function opens the log file if it is not already open and checks
 * if the current log file has reached the maximum size. If so, the current
 * log file is closed and a new log file is opened.
 *
 * @param level The log level of the message (DEBUG, FINE, INFO, WARNING, ERROR).
 * @param module The name of the module writing the log message.
 * @param format The format of the log message, similar to printf.
 * @param ... Additional arguments used in the format string.
 */
void log_msg(const LogLevel level, const char *module, const char *format, ...) {
    //open log file for the first time in a session
    if (log_file == nullptr) {
        file_id = get_latest_file_id();
        if (file_id != -1) {
            open_log_file();
        }
    }
    check_log_file();

    //get timestamp
    const time_t now = time(nullptr);
    const struct tm *tm = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), TIMESTAMP_FORMAT, tm);

    //get log level
    const char *log_level = log_level_str[level];

    va_list args;
    va_start(args, format);

    if (log_file) {
        //temp msg placeholder
        char msg[1024];
        vsnprintf(msg, sizeof(msg), format, args);

        fprintf(log_file, MSG_FORMAT, timestamp, log_level, module, msg);
        fflush(log_file);
    }

    va_end(args);
}