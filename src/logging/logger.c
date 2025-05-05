#include "logger.h"

#include "ringbuffer.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
    #include <direct.h>
    #include <sys/stat.h>// For _stat
    #include <windows.h>
    #define STAT_STRUCT struct _stat
    #define STAT_FUNC _stat
    #define MKDIR(path) _mkdir(path)
    #define PATH_SEP "\\"
#else
    #include <dirent.h>
    #include <sys/stat.h>
    #define STAT_STRUCT struct stat
    #define STAT_FUNC stat
    #define MKDIR(path) mkdir(path, 0755)
    #define PATH_SEP "/"
#endif

#define MAX_N_FILES 5
#define MAX_FILE_SIZE (100 * 1024)// max size = 100 kB
#define PATH_MAX 4096

// relative directory from the project root
#define LOG_DIRECTORY "log"
#define LOG_FILE_FORMAT "log-%d.txt"

#define TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"
#define MSG_FORMAT "[%s] [%s] [%s] : %s\n"

#define MAX_HEADER_SIZE 512

#define FAILED (-1)
#define SUCCESS 0

#define logger_not_init_return(ret) \
    if (logger_is_running == false) { return ret; }

const char* log_level_str[] = {"DEBUG", "FINE", "INFO", "WARNING", "ERROR"};

// === internal functions ===
void start_log_writer_thread(void);
void close_log_file(bool terminate_thread);

FILE* log_file = NULL;
ring_buffer_t log_buffer;

//states if the file writing thread is still running, if set to false, the thread terminates or is terminated
bool logger_is_running = false;
//the id of the used file
int file_id = 0;

/**
 * Ensures that the predefined log directory already exists,
 * if not create a new one.
 *
 * @return -1 if the directory could not be created, 0 if the directory already exists or was created successfully
 */
int ensure_log_dir(void) {
    STAT_STRUCT st;

    if (STAT_FUNC(LOG_DIRECTORY, &st) == -1) {
        if (MKDIR(LOG_DIRECTORY) == -1) {
            return FAILED;
        }
    }
    return SUCCESS;
}


/**
 * Opens the log file with the current saved file id in appended modus.
 * If no file is found, creates a new file corresponding to fopen(...).
 *
 * @return -1 if the file could not be open, 0 when successfully
 */
int open_log_file(void) {
    char name[16];
    snprintf(name, sizeof(name), LOG_FILE_FORMAT, file_id);

    char filename[256];
    snprintf(filename, sizeof(filename), "%s" PATH_SEP "%s", LOG_DIRECTORY, name);

    log_file = fopen(filename, "a");
    if (!log_file) {
        return FAILED;
    }
    return SUCCESS;
}

/**
 * This function should be called whenever a new log msg must be written.
 *
 * Check if the log file is open, if not, a new file will be open.
 * - Either by creating a new file if no file is in the log directory.
 * - Or creating a new file if the last used file reaches the max size.
 *   The new file will get the current file id + 1 or 0, if the id reached the max number of files.
 * - Or opening the file that was last used, the current id will be set.
 */
void check_log_file(void) {
    if (log_file) {
        fseek(log_file, 0, SEEK_END);
        const long file_size = ftell(log_file);
        if (file_size >= MAX_FILE_SIZE) {
            close_log_file(false);
            file_id = (file_id + 1) % MAX_N_FILES;
            if (open_log_file() == FAILED) {
                close_log_file(true);
            }
        }
    }
}

int get_latest_file_id(void) {
    if (ensure_log_dir() == FAILED) {
        return FAILED;
    }

    int latest_id = 0;

#ifdef _WIN32
    WIN32_FIND_DATA find_data;
    HANDLE hFind;
    FILETIME latest_time = {0};

    char search_path[PATH_MAX];
    snprintf(search_path, sizeof(search_path), "%s" PATH_SEP "log-*.txt", LOG_DIRECTORY);

    hFind = FindFirstFile(search_path, &find_data);
    if (hFind == INVALID_HANDLE_VALUE) {
        return 0;
    }

    do {
        int id;
        if (sscanf(find_data.cFileName, LOG_FILE_FORMAT, &id) == 1 && id >= 0 && id < MAX_N_FILES) {
            if (CompareFileTime(&find_data.ftLastWriteTime, &latest_time) > 0) {
                latest_time = find_data.ftLastWriteTime;
                latest_id = id;
            }
        }
    } while (FindNextFile(hFind, &find_data) != 0);

    FindClose(hFind);
#else
    DIR* dir = opendir(LOG_DIRECTORY);
    struct dirent* entry;
    time_t latest_time = 0;

    if (!dir) {
        return FAILED;
    }

    while ((entry = readdir(dir)) != NULL) {
        int id;
        struct stat file_stat;

        if (sscanf(entry->d_name, LOG_FILE_FORMAT, &id) == 1 && id >= 0 && id < MAX_N_FILES) {
            char filepath[PATH_MAX];
            snprintf(filepath, sizeof(filepath), "%s" PATH_SEP "%s", LOG_DIRECTORY, entry->d_name);

            if (stat(filepath, &file_stat) == 0) {
                if (file_stat.st_mtime > latest_time) {
                    latest_time = file_stat.st_mtime;
                    latest_id = id;
                }
            }
        }
    }

    closedir(dir);
#endif

    return latest_id;
}

void close_log_file(const bool terminate_thread) {
    if (log_file != NULL) {
        fclose(log_file);
        log_file = NULL;
    }
    if (terminate_thread) {
        logger_is_running = false;
        free_ringbuffer(&log_buffer);
    }
}

void init_logger(void) {
    if (log_file == NULL) {
        if (init_ringbuffer(&log_buffer) == 0) {
            file_id = get_latest_file_id();
            if (file_id == FAILED || open_log_file() == FAILED) {
                close_log_file(true);
            } else {
                start_log_writer_thread();
            }
        }
    }
}

/**
 * Writes a log message to the log file. If the logger was not initialized, it will return immediately.
 *
 * @param level The log level of the message (DEBUG, FINE, INFO, WARNING, ERROR).
 * @param module The name of the module writing the log message.
 * @param format The format of the log message, similar to printf.
 * @param ... Additional arguments used in the format string.
 */
void log_msg(const log_level_t level, const char* module, const char* format, ...) {
    logger_not_init_return();

    //get timestamp
    const time_t now = time(NULL);

#ifdef _WIN32
    struct tm tm_buf;
    localtime_s(&tm_buf, &now);
    const struct tm* tm = &tm_buf;
#else
    const struct tm* tm = localtime(&now);
#endif

    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), TIMESTAMP_FORMAT, tm);

    const char* log_level;
    if (level >= MAX_LOG_LEVEL) {
        log_level = log_level_str[INFO];
    } else {
        log_level = log_level_str[level];
    }

    va_list args;
    va_start(args, format);
    char msg[MAX_HEADER_SIZE];
    vsnprintf(msg, sizeof(msg), format, args);

    char log_msg[MAX_MSG_LENGTH];
    snprintf(log_msg, MAX_MSG_LENGTH, MSG_FORMAT, timestamp, log_level, module, msg);
    va_end(args);

    if (log_file != NULL && logger_is_running) {
        //if the log file is open and the thread is running
        write_to_ringbuffer(&log_buffer, log_msg);
    }
}

void shutdown_logger(void) {
    close_log_file(true);
}

#ifdef _WIN32
DWORD WINAPI log_writer_thread(LPVOID param) {
    bool running = true;
    while (running) {
        char log_msg[MAX_MSG_LENGTH];
        if (read_from_ringbuffer(&log_buffer, log_msg) == 0) {
            // message successfully read from the ringbuffer
            check_log_file();
            if (log_file) {
                fprintf(log_file, "%s", log_msg);
                fflush(log_file);
            }
        } else if (!logger_is_running) {
            // thread is not running anymore and the ringbuffer is empty
            running = false;
        }
    }
    return 0;
}
#else
/**
 * This function will be called from a different thread to read from the ringbuffer
 * and then write in the log file
 *
 * @param arg A pointer for different arguments, set by the thread. Will not be used!
 * @return NULL
 */
void* log_writer_thread(void* arg) {
    bool running = true;
    while (running) {
        char log_msg[MAX_MSG_LENGTH];
        if (read_from_ringbuffer(&log_buffer, log_msg) == 0) {
            // message successfully read from the ringbuffer
            check_log_file();
            if (log_file) {
                fprintf(log_file, "%s", log_msg);
                fflush(log_file);
            }
        } else if (!logger_is_running) {
            // thread is not running anymore and the ringbuffer is empty
            running = false;
        }
    }
    return NULL;
}
#endif

void start_log_writer_thread(void) {
    logger_is_running = true;
#ifdef _WIN32
    HANDLE thread = CreateThread(NULL, 0, log_writer_thread, NULL, 0, NULL);
    if (thread) {
        CloseHandle(thread);
    }
#else
    pthread_t thread;
    pthread_create(&thread, NULL, log_writer_thread, NULL);
    pthread_detach(thread);
#endif
}
