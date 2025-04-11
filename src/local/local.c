#include "local.h"

#include "../database/local/local_database.h"

#include <stdlib.h>

// forward declaration of the observer node structure
typedef struct observer_node_t observer_node_t;

/**
 * List node for the observer list
 */
typedef struct observer_node_t {
    update_observer_t update_func;//function pointer to the observer
    observer_node_t* next;
} observer_node_t;

/**
 * The pointer to the head of the observer list
 */
observer_node_t* observer_list = NULL;

/**
 * The current language setting
 */
local_language_t current_language = LANGUAGE_EN;

/**
 * Initialize the local module.
 * This function should be called before using any other functions in this module.
 */
void init_local(void) {
    // Initialize the observer list
    observer_list = malloc(sizeof(observer_node_t));
    if (observer_list == NULL) {
        // Handle memory allocation failure
        return;
    }
    observer_list->update_func = NULL;
    observer_list->next = NULL;
}

/**
 * Get the localized string for the given key.
 * @param key the key for the localized string
 * @return the localized string
 */
char* get_local_string(const char* key) {
    // This function should return the localized string for the given key
    // TODO: Implement the DB Connection, for now it is NULL
    return get_localization_string(NULL, key, &current_language);
}

/**
 * Set the current language.
 * @param local_lang the language to set
 * @return true if the language was set successfully, false otherwise
 */
bool set_language(const local_language_t local_lang) {
    if (local_lang.lang >= MAX_LANG) {
        return false;// Invalid language
    }
    current_language = local_lang;

    // go through the observer list and call each observer function
    const observer_node_t* current = observer_list;
    while (current != NULL) {
        if (current->update_func != NULL) {
            current->update_func(current_language);// call the observer function
        }
        current = current->next;
    }

    return true;
}

/**
 * Add an observer to the list.
 * @param update_func the function to be called when the language is updated
 */
void add_observer(const update_observer_t update_func) {
    if (update_func == NULL || observer_list == NULL) {
        // TODO: Error handling
        return;
    }

    observer_node_t* new_node = malloc(sizeof(observer_node_t));
    if (new_node == NULL) {
        // Handle memory allocation failure
        return;
    }
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
    // free the observer list
    observer_node_t* current = observer_list;
    while (current != NULL) {
        observer_node_t* next = current->next;
        free(current);
        current = next;
    }
}
