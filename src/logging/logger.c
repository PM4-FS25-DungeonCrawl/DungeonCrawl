#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>

#include "logger.h"
#include "ringbuffer.h" //includes pthread.h or windows.h

#define MAX_N_FILES 5
#define MAX_FILE_SIZE 1048576 // max size = 1 MB

// relative directory from main.c
#define LOG_DIRECTORY "../log"
#define LOG_FILE_FORMAT "log-%d.txt"

#define TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"
#define MSG_FORMAT "[%s] [%s] [%s] : %s\n"

#define MAX_HEADER_SIZE 256

#define RUNNING 1
#define TERMINATED 0

const char *log_level_str[] = {"DEBUG", "FINE", "INFO", "WARNING", "ERROR"};

//predefines of functions, used in this module
void start_log_writer_thread(void);

//the used file-pointer to write the log messages in
FILE *log_file = nullptr;
//the used RingBuffer to write log messages in
RingBuffer log_buffer;

//states if the file writing thread is still running, if set to 0 the thread terminates or is terminated
int thread_is_running = TERMINATED;
//the id of the used file
int file_id = 0;

/**
 * ensures that the predefined log directory already exist,
 * if not create a new one
 */
void ensure_log_dir(void) {
    struct stat st;

    if (stat(LOG_DIRECTORY, &st) == -1) {
        //Directory doesn't exist -> create a new one

        if (mkdir(LOG_DIRECTORY, 0755) == -1) {
            //directory konnte nicht erstellt werden
            //TODO: Errorhandling?
        }
    }
}



/**
 * Opens the log file with current saved file id in append modus.
 * If no file is found creates a new file corresponding to fopen(...).
 *
 * If the file could not be open or created, this method will do nothing
 */
void open_log_file(void) {
    char name[15];
    snprintf(name, 15, LOG_FILE_FORMAT, file_id);

    char filename[256];
    snprintf(filename, 256, "%s/%s", LOG_DIRECTORY, name);

    //open file in append modus
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
            close_log_file(0);
            file_id = (file_id + 1) % MAX_N_FILES;
            open_log_file();
        }
    }
}

/**
 * Gets from the file (last used) in the log directory, the id.
 * @return the latest used file id, in the range 0 (ink.) to MAX_N_FILES (exkl.) or -1 if directory was not found.
 */
int get_latest_file_id(void) {
    ensure_log_dir();

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
 *
 * @param terminate_thread if 1 the running thread will also be terminated
 */
void close_log_file(const int terminate_thread) {
    if (log_file) {
        fclose(log_file);
        log_file = nullptr;
    }
    if (terminate_thread && thread_is_running) {
        thread_is_running = TERMINATED;
    }
}

/**
 * Writes a log message to the log file.
 *
 * If it is the first call of the session the ring buffer will be initialized
 * and the writing thread will be started.
 *
 * @param level The log level of the message (DEBUG, FINE, INFO, WARNING, ERROR).
 * @param module The name of the module writing the log message.
 * @param format The format of the log message, similar to printf.
 * @param ... Additional arguments used in the format string.
 */
void log_msg(const LogLevel level, const char *module, const char *format, ...) {
    //starting thread
    if (log_file == nullptr) {
        init_ring_buffer(&log_buffer); // init ring buffer to write the message in

        start_log_writer_thread(); //start thread
    }

    //if thread is not running, something went wrong with opening the log dir, log file
    if (thread_is_running) {
        //get timestamp
        const time_t now = time(nullptr);
        const struct tm *tm = localtime(&now);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), TIMESTAMP_FORMAT, tm);

        //get log level
        const char *log_level = log_level_str[level];

        va_list args;
        va_start(args, format);
        //temp msg placeholder
        char msg[MAX_HEADER_SIZE];
        vsnprintf(msg, sizeof(msg), format, args);

        char log_msg[MAX_MSG_LENGTH];
        snprintf(log_msg, MAX_MSG_LENGTH, MSG_FORMAT, timestamp, log_level, module, msg);
        va_end(args);

        write_to_ring_buffer(&log_buffer, log_msg);
    }
}

#ifdef _WIN32
    /**
     * This function will be called from a different thread to read from the ringbuffer
     * and then write in the log file
     *
     * @param arg a pointer for different arguments, set by the thread. Will not be used!
     * @return 0
     */
    DWORD WINAPI log_writer_thread(LPVOID param) {
        while (thread_is_running) {
            char log_msg[MAX_MSG_LENGTH];
            if (read_from_ring_buffer(&log_buffer, log_msg)) {
                // message successfully read from ringbuffer

                // open log file for the first time this session
                if (log_file == nullptr) {
                    file_id = get_latest_file_id();
                    if (file_id != -1) {
                        //if the file id could be defined, doesn't try to open the file
                        open_log_file();
                    }
                }
                check_log_file();


                if (log_file) {
                    //writes to the log file
                    fprintf(log_file, "%s", log_msg);
                    fflush(log_file);
                }
            }
        }
        return 0;
    }
#else
    /**
     * This function will be called from a different thread to read from the ringbuffer
     * and then write in the log file
     *
     * @param arg a pointer for different arguments, set by the thread. Will not be used!
     * @return NULL
     */
    void *log_writer_thread(void *arg) {
        while (thread_is_running) {
            char log_msg[MAX_MSG_LENGTH];
            if (read_from_ring_buffer(&log_buffer, log_msg)) {
                // message successfully read from ringbuffer

                // open log file for the first time this session
                // only tries once -> no logs will be created
                if (log_file == nullptr && file_id != -1) {
                    file_id = get_latest_file_id();
                    if (file_id != -1) {
                        //if the file id could be defined, opens the file
                        open_log_file();
                    } else {
                        //stops the thread from running, no logs will be created
                        //TODO: Errorhandling?
                        thread_is_running = TERMINATED;
                    }
                }
                check_log_file();


                if (log_file) {
                    //writes to the log file
                    fprintf(log_file, "%s", log_msg);
                    fflush(log_file);
                }
            }
        }
        return NULL;
    }
#endif

/**
 * Starts the logging writing thread
 */
void start_log_writer_thread(void) {
    thread_is_running = RUNNING;
    #ifdef _WIN32
        HANDLE thread = CreateThread(NULL, 0, log_writer_thread, NULL, 0, NULL);
        if (thread) {
            CloseHandle(thread);
        }
    #else
        pthread_t thread;
        pthread_create(&thread, nullptr, log_writer_thread, NULL);
        pthread_detach(thread);
    #endif
}