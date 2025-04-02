#ifndef ITEMS_H
#define ITEMS_H

#include <stdio.h>
#include <string.h>

typedef enum {
    ITEM_TYPE_USABLE,
    ITEM_TYPE_EQUIPPABLE
} item_type_t;

typedef enum {
    SLOT_HEAD,
    SLOT_CHEST,
    SLOT_LEGS,
    SLOT_FEET,
    SLOT_HANDS,
    SLOT_NECK,
    SLOT_FINGER_RIGHT,
    SLOT_FINGER_LEFT,
    SLOT_COUNT
} gear_slot_t;

typedef enum {
    HEALING,
    ARMOR_INCREASE
    // more effects can be added
} usable_item_effect_t;

// Base Item
typedef struct {
    char name[50];
    item_type_t type;
} item_t;

// Equipable Item
typedef struct {
    item_t base;
    gear_slot_t slot;
    int armor_bonus;
    // Armor Pieces can have other stats, e.g. +might etc. for now only armor
} equipable_item_t;

// Usable Item
typedef struct {
    item_t base;
    usable_item_effect_t effectType;
    int value; // e.g. value = 30, healing potion heals for 30 health
    // for other item effects more values might be needed
} usable_item_t;

// Function Declarations
void init_usable_item(usable_item_t *item, const char *name, usable_item_effect_t effectType, int value);
void init_equipable_item(equipable_item_t *item, const char *name, gear_slot_t slot, int armor_bonus);

#endif // ITEMS_H
