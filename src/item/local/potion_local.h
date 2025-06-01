#ifndef POTION_LOCAL_H
#define POTION_LOCAL_H

/**
 * @var potion_names
 * Global array of strings representing the names of potion items.
 */
extern char** potion_names;

/**
 * @brief Initializes the local potion system.
 *
 * This function sets up any resources or state required for managing potions locally.
 *
 * @return int Returns 0 on success, or a non-zero error code on failure.
 */
int init_potion_local(void);

/**
 * @brief Shuts down the local potion system.
 *
 * This function releases any resources or state associated with the local potion system.
 */
void shutdown_potion_local(void);

#endif//POTION_LOCAL_H
