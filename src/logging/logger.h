#ifndef LOGGER_H
#define LOGGER_H

#define DEBUG_STATE 1//change to 0 to disable debug logging

#define DEBUG_LOG(modul, format, ...)                 \
    if (DEBUG_STATE == 1) {                           \
        log_msg(DEBUG, modul, format, ##__VA_ARGS__); \
    }

#define RETURN_WHEN_NULL(ptr, ret, modul, format, ...) \
    if (ptr == NULL) {                                 \
        log_msg(ERROR, modul, format, ##__VA_ARGS__);  \
        return ret;                                    \
    }

#define RETURN_WHEN_TRUE(expr, ret, modul, format, ...) \
    if (expr) {                                         \
        log_msg(ERROR, modul, format, ##__VA_ARGS__);   \
        return ret;                                     \
    }

typedef enum {
    DEBUG,
    FINE,
    INFO,
    WARNING,
    ERROR,
    MAX_LOG_LEVEL
} log_level_t;

/**
 * Initializes the logging system for the application.
 *
 * This function sets up the logging system to be ready for recording log entries.
 * It creates or opens the log file, initializes the ring buffer used for storing
 * log entries temporarily, and starts the thread that asynchronously writes logs
 * to the file. This ensures logging is operational and ready to use for the
 * application's lifetime.
 *
 * Notes:
 * - This function can only be called once during the application's lifecycle.
 *   Subsequent calls will have no effect if the logger is already initialized.
 * - All required resources for logging, such as the log file and ring buffer, are
 *   prepared within this initialization process.
 * - If any of the initialization steps fail (e.g., ring buffer initialization or
 *   file operations), the function ensures cleanup of allocated resources.
 */
void init_logger(void);

/**
 * Logs a formatted message with a specified log level and module.
 *
 * This function generates a log entry that includes a timestamp, log level, module name,
 * and a custom message formatted using a variable argument list. The log message is
 * written to a ring buffer for asynchronous processing.
 *
 * Notes:
 * - The logging system must be initialized and running before using this function.
 *   Otherwise, the function will return without performing any operations.
 * - The log level determines the severity or importance of the message.
 *
 * @param level The severity level of the log message (e.g., DEBUG, INFO, ERROR).
 *              If the provided log level exceeds the maximum defined levels, the INFO
 *              level will be used as a fallback.
 * @param module A string identifying the module or component generating the log message.
 * @param format A printf-style format string for the message content.
 * @param ... Additional arguments to be formatted into the log message according
 *            to the format string.
 */
void log_msg(log_level_t level, const char* module, const char* format, ...);

/**
 * Shuts down the logging system for the application.
 *
 * This function terminates the logging system, ensuring that no further
 * log entries are recorded. It sets the logger's running state to false,
 * effectively disabling any ongoing or future logging operations.
 *
 * Use this function to cleanly release logging resources and mark the
 * completion of logging operations at the end of the application's lifecycle
 * or during application shutdown sequences.
 *
 * Notes:
 * - Once this function is called, logging within the application will cease
 *   to function.
 * - This function should be called after all dependent systems and processes
 *   using the logger are finalized.
 */
void shutdown_logger(void);
#endif//LOGGER_H
