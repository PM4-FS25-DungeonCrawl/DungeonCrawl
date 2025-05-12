#ifndef LOCAL_H
#define LOCAL_H

#include "../common.h"

typedef enum {
    LANG_EN,
    LANG_DE,
    MAX_LANG
} language_t;

typedef struct {
    language_t lang;
    char* column_name;// from database
} local_language_t;

#define LANGUAGE_EN {LANG_EN, "LC_TEXT_EN"}

static const local_language_t language_en = {LANG_EN, "LC_TEXT_EN"};
static const local_language_t language_de = {LANG_DE, "LC_TEXT_DE"};

//function pointer type for update observer
typedef void (*update_observer_t)(void);

extern string_max_t* local_strings;

/**
 * Initializes the local module and its associated resources.
 *
 * This function performs the following tasks:
 * - Allocates memory for the observer list and initializes it.
 * - Allocates memory for storing local strings.
 * - Establishes a database connection to the local database.
 *
 * @return 0 if initialization is successful, or a non-zero value
 * indicating the cause of the failure.
 */
int init_local(void);

char* get_local_string(const char* key);
bool set_language(local_language_t local_lang);

void add_local_observer(update_observer_t update_func);

/**
 * Shuts down the local module and releases associated resources.
 *
 * This function performs the following tasks:
 * - Frees memory allocated for local strings.
 * - Iterates through and deallocates all nodes in the observer list, if any exist.
 * - Closes the local database connection.
 *
 * If the local module has not been initialized (indicated by a NULL observer list),
 * the function returns immediately without performing any actions.
 *
 * Note: This function is intended to be called during the shutdown process
 * to ensure proper cleanup of resources used by the local module.
 */
void shutdown_local(void);

#endif//LOCAL_H
