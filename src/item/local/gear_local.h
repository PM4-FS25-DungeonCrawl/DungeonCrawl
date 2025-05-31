#ifndef GEAR_LOCAL_H
#define GEAR_LOCAL_H

/**
 * @var gear_names
 * Global array of strings representing the names of gear items.
 */
extern char** gear_names;

/**
 * @brief Initializes the local gear system.
 *
 * This function sets up any resources or state required for managing gear locally.
 *
 * @return int Returns 0 on success, or a non-zero error code on failure.
 */
int init_gear_local(void);

/**
 * @brief Shuts down the local gear system.
 *
 * This function releases any resources or state associated with the local gear system.
 */
void shutdown_gear_local(void);

#endif //GEAR_LOCAL_H
