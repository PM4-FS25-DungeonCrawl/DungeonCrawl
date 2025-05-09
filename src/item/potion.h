#ifndef POTION_H
#define POTION_H

#include "../common.h"
#include "../memory/memory_management.h"

#include "../database/database.h"

typedef enum {
    HEALING,
    MANA,
    STAMINA,
    MAX_POTION_TYPES
} potion_type_t;

typedef struct {
    char name[MAX_NAME_LENGTH];
    potion_type_t effectType;
    int value;// e.g. value = 30, healing potion heals for 30 health
    // for other item effects more values might be needed
} potion_t;

typedef struct {
    potion_t potions[MAX_POTION_TYPES];
} potion_table_t;

potion_t* init_potion(potion_t* potion, const char* name, potion_type_t type, int value);
potion_table_t* init_potion_table(memory_pool_t* memory_pool, const db_connection_t* db_connection);
const char* potion_type_to_string(potion_type_t type);
void free_potion(memory_pool_t* memory_pool, potion_t* potion);
void free_potion_table(memory_pool_t* memory_pool, potion_table_t* table);

#endif//POTION_H
