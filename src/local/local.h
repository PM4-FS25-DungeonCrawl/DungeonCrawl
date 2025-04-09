#ifndef LOCAL_H
#define LOCAL_H

typedef enum {
    LANG_EN,
    LANG_DE,
    LANG_MAX
} language_t;

typedef struct {
    language_t lang;
    char* column_name;// from database
} local_language_t;

#define LANGUAGE_DE {LANG_DE, "LC_TEXT_DE"}
#define LANGUAGE_EN {LANG_EN, "LC_TEXT_EN"}
#define LANGUAGE_MAX {LANG_MAX, "LC_TEXT_EN"}// english if there is not a valid language


//function pointer type for update observer
typedef void (*update_observer_t)(language_t lang);

void local_init(void);

char* get_local_string(const char* key);
bool set_language(language_t lang);

void add_observer(update_observer_t update_func);

void local_shutdown(void);

#endif//LOCAL_H
