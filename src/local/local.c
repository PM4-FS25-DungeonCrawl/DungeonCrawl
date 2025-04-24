#include "local.h"

#include "../common.h"
#include "../database/database.h"
#include "../database/local/local_database.h"

#include <stdlib.h>

//Macros for the Local Database Path
#define LOCAL_DB_PATH "resources/database/local/dungeoncrawl_local.db"

#define local_not_init_return(ret) if (observer_list == NULL) { return ret; }

// forward declaration of the observer node structure
typedef struct observer_node_t observer_node_t;

/**
 * List node for the observer list
 */
typedef struct observer_node_t {
    update_observer_t update_func;//function pointer to the observer
    observer_node_t* next;
} observer_node_t;


// === internal global variables ===
/**
 * The pointer to the head of the observer list
 */
observer_node_t* observer_list = NULL;

/**
 * The current language setting
 */
local_language_t current_language = LANGUAGE_EN;

/**
 * The database connection
 */
db_connection_t local_db_connection;

/**
 * @brief Initialize the local module.
 * @return 0 on success, non-zero on failure
 * @note This function must be called before using any other functions in this module.
 */
int init_local(void) {
    // Initialize the observer list
    observer_list = malloc(sizeof(observer_node_t));
    NULL_PTR_HANDLER_RETURN(observer_list, 1, "Local", "Failed to allocate memory for observer_list");

    observer_list->update_func = NULL;
    observer_list->next = NULL;
    // Initialize the database connection
    return db_open(&local_db_connection, LOCAL_DB_PATH);
}

/**
 * Get the localized string for the given key.
 * @param key the key for the localized string
 * @return the localized string
 */
char* get_local_string(const char* key) {
    local_not_init_return("local module not initialized");
    // This function should return the localized string for the given key
    return get_localization_string(&local_db_connection, key, &current_language);
}

/**
 * Set the current language.
 * @param local_lang the language to set
 * @return true if the language was set successfully, false otherwise
 */
bool set_language(const local_language_t local_lang) {
    local_not_init_return(false);

    if (local_lang.lang >= MAX_LANG) {
        log_msg(WARNING, "Local", "Failed to set local language with invalid language: %d", local_lang.lang);
        return false;// Invalid language
    }
    current_language = local_lang;

    // go through the observer list and call each observer function
    const observer_node_t* current = observer_list;
    while (current != NULL) {
        if (current->update_func != NULL) {
            current->update_func();// call the observer function
        }
        current = current->next;
    }

    return true;
}

/**
 * Add an observer to the list.
 * @param update_func the function to be called when the language is updated
 */
void add_local_observer(const update_observer_t update_func) {
    local_not_init_return();
    NULL_PTR_HANDLER_RETURN(update_func, , "Local", "Observer function is NULL");

    observer_node_t* new_node = malloc(sizeof(observer_node_t));
    NULL_PTR_HANDLER_RETURN(new_node, , "Local", "Failed to allocate memory for new observer node");

    new_node->update_func = update_func;
    // add at the end of the list
    new_node->next = NULL;
    observer_node_t* current = observer_list;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;
}


/**
 * Shutdown the local module and frees any allocated resources.
 * This function should be called when the module is no longer needed.
 */
void shutdown_local(void) {
    local_not_init_return();
    // free the observer list
    observer_node_t* current = observer_list;
    while (current != NULL) {
        observer_node_t* next = current->next;
        free(current);
        current = next;
    }
    db_close(&local_db_connection);
}
