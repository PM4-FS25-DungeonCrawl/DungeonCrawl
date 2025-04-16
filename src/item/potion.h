#ifndef POTION_H
#define POTION_H

#include "../common.h"

typedef enum {
    HEALING,
    // more effects can be added
    MAX_POTION_TYPES
} potion_type_t;

typedef struct {
    char name[MAX_NAME_LENGTH];
    potion_type_t effectType;
    int value;// e.g. value = 30, healing potion heals for 30 health
    // for other item effects more values might be needed
} potion_t;

potion_t* init_potion(const char* name, potion_type_t type, int value);
const char* potion_type_to_string(potion_type_t type);
void free_potion(potion_t* item);

#endif//POTION_H
