#ifndef USABLE_ITEM_H
#define USABLE_ITEM_H

typedef enum {
    HEALING,
    ARMOR_INCREASE
    // more effects can be added
} usable_item_effect_t;

typedef struct {
    item_t* base; // base item structure
    usable_item_effect_t effectType;
    int value; // e.g. value = 30, healing potion heals for 30 health
    // for other item effects more values might be needed
} usable_item_t;

usable_item_t* init_usable_item(const char* name, usable_item_effect_t effectType, int value);
void free_usable_item(usable_item_t* item);

#endif //USABLE_ITEM_H
