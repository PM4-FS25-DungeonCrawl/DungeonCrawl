#ifndef LOCAL_H
#define LOCAL_H

typedef enum {
    LANG_EN,
    LANG_DE,
    LANG_MAX
} language_t;

//function pointer type for update observer
typedef void (*update_observer_t)(language_t lang);

void local_init(void);

char* get_local_string(const char* key);
bool set_language(language_t lang);

void add_observer(update_observer_t update_func);

void local_shutdown(void);

#endif//LOCAL_H
