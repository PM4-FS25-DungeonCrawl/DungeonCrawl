#ifndef ITEMS_H
#define ITEMS_H

#include <stdio.h>
#include <string.h>

typedef enum {
    ITEM_TYPE_USABLE,
    ITEM_TYPE_EQUIPPABLE
} ItemType;

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
} GearSlot;

typedef enum {
    HEALING,
    ARMOR_INCREASE
    // more effects can be added
} UsableItemEffectType;

// Base Item
typedef struct {
    char name[50];
    ItemType type;
} Item;

// Equippable Item
typedef struct {
    Item base;
    GearSlot slot;
    int armor_bonus;
    // Armor Pieces can have other stats, e.g. +might etc. for now only armor
} EquippableItem;

// Usable Item
typedef struct {
    Item base;
    UsableItemEffectType effectType;
    int value;// e.g. value = 30, healing potion heals for 30 health
              // for other item effects more values might be needed
} UsableItem;

// Function Declarations
void init_usable_item(UsableItem* item, const char* name, UsableItemEffectType effectType, int value);
void init_equippable_item(EquippableItem* item, const char* name, GearSlot slot, int armor_bonus);

#endif// ITEMS_H
