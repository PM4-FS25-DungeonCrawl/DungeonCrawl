#ifndef BASE_ITEM_H
#define BASE_ITEM_H

#include "../common.h"
#include "equipable_item.h"
#include "usable_item.h"

typedef enum {
    USABLE,
    EQUIPABLE
} item_type_t;

typedef struct {
    char name[MAX_NAME_LENGTH];
    item_type_t type;
    void* extension; // pointer to either usable_item_t or equipable_item_t
} item_t;

item_t* init_item(const char* name, item_type_t type, void* extension);
void free_item(item_t* item);

#endif //BASE_ITEM_H
