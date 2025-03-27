//
// Created by ninof on 27.03.2025.
//

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

#include "logger.h"

const char *log_level_str[] = {"DEBUG", "FINE", "INFO", "WARNING", "ERROR"};

char filename[256];
FILE *log_file = nullptr;

int current_file_id = 0;

void open_log_file(void) {

    //open file in append modus
    char name[15];
    snprintf(name, 15, LOG_FILE_FORMAT, current_file_id);

    snprintf(filename, 256, "%s%s", LOG_DIRECTORY, name);

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
        if (file_size < MAX_FILE_SIZE) {
            return;
        }
        close_log_file();
        current_file_id = (current_file_id + 1) % MAX_N_FILES;
    }

    // Öffnen einer neuen Logdatei
    open_log_file();
}

/**
 * Gets from the file (last used) in the log directory, the id.
 */
int get_current_file_id(void) {
    DIR *dir = opendir(LOG_DIRECTORY);
    struct dirent *entry;
    int max_id = 0;

    if (!dir) {
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        int id;
        if (strtol(entry->d_name, LOG_FILE_FORMAT, &id) == 1) {
            if (id > max_id) {
                max_id = id;
            }
        }
    }

    closedir(dir);

    return max_id == -1 ? 0 : max_id;
}

void close_log_file(void) {
    if (log_file) {
        fclose(log_file);
        log_file = nullptr;
    }
}


void log_msg(const LogLevel level, const char *module, const char *format, ...) {
    //open log file once
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