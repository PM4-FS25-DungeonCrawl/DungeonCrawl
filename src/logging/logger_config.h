/**
 * @file logger_config.h
 * @brief Configuration file for the logger.
 */

#ifndef LOGGER_CONFIG_H
#define LOGGER_CONFIG_H

#define MAX_N_FILES 5
#define MAX_FILE_SIZE (100 * 1024)// max size = 100 kB

#define MAX_HEADER_SIZE 256
#define MAX_MSG_SIZE (512 + MAX_HEADER_SIZE)

#endif//LOGGER_CONFIG_H
