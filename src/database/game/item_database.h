#ifndef ITEM_DATABASE_H
#define ITEM_DATABASE_H

#include "../database.h"
#include "../../item/gear.h"
#include "../../item/potion.h"
#include "../../character/stats.h"

/**
 * To get
 */
typedef struct potion_init_t {
    int potion_type; // The index of the potion in the table
    char* name;
    int value;
} potion_init_t;

#endif //ITEM_DATABASE_H
