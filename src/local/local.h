#ifndef LOCAL_H
#define LOCAL_H

typedef enum {
    LANG_EN,
    LANG_DE,
    MAX_LANG
} language_t;

typedef struct {
    language_t lang;
    char* column_name;// from database
} local_language_t;

typedef struct {
    int idx;
    char* key;
} local_key_t;

#define LANGUAGE_EN {LANG_EN, "LC_TEXT_EN"}
#define LANGUAGE_DE {LANG_DE, "LC_TEXT_DE"}

//function pointer type for update observer
typedef void (*update_observer_t)(void);

int init_local(void);

char* get_local_string(const char* key);
bool set_language(local_language_t local_lang);

void add_local_observer(update_observer_t update_func);

void shutdown_local(void);

#endif//LOCAL_H
