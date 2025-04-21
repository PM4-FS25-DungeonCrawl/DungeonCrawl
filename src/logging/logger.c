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
#define MAX_FILE_SIZE 1048576// max size = 1 MB
#define PATH_MAX 4096

#define LOG_DIRECTORY "log"
#define LOG_FILE_FORMAT "log-%d.txt"

#define TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"
#define MSG_FORMAT "[%s] [%s] [%s] : %s\n"

#define MAX_HEADER_SIZE 512

#define FAILED (-1)
#define SUCCESS 0

const char* log_level_str[] = {"DEBUG", "FINE", "INFO", "WARNING", "ERROR"};

void start_log_writer_thread(void);
void close_log_file(bool terminate_thread);

FILE* log_file = NULL;
ring_buffer_t log_buffer;

bool thread_is_running = false;
int file_id = 0;

int ensure_log_dir(void) {
    STAT_STRUCT st;

    if (STAT_FUNC(LOG_DIRECTORY, &st) == -1) {
        if (MKDIR(LOG_DIRECTORY) == -1) {
            return FAILED;
        }
    }
    return SUCCESS;
}

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
        thread_is_running = false;
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

void log_msg(const log_level_t level, const char* module, const char* format, ...) {
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

    if (log_file != NULL && thread_is_running) {
        write_to_ringbuffer(&log_buffer, log_msg);
    } else {
        printf("%s", log_msg);
    }
}

void shutdown_logger(void) {
    close_log_file(true);
}

#ifdef _WIN32
DWORD WINAPI log_writer_thread(LPVOID param) {
    while (thread_is_running) {
        char log_msg[MAX_MSG_LENGTH];
        if (read_from_ringbuffer(&log_buffer, log_msg) == 0) {
            check_log_file();
            if (log_file) {
                fprintf(log_file, "%s", log_msg);
                fflush(log_file);
            }
        }
    }
    return 0;
}
#else
void* log_writer_thread(void* arg) {
    while (thread_is_running) {
        char log_msg[MAX_MSG_LENGTH];
        if (read_from_ringbuffer(&log_buffer, log_msg) == 0) {
            check_log_file();
            if (log_file) {
                fprintf(log_file, "%s", log_msg);
                fflush(log_file);
            }
        }
    }
    return NULL;
}
#endif

void start_log_writer_thread(void) {
    thread_is_running = true;
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
