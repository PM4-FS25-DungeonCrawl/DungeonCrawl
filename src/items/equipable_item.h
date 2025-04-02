#ifndef EQUIPABLE_ITEM_H
#define EQUIPABLE_ITEM_H


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

typedef struct {
    char name[50];
    gear_slot_t slot;
    int armor_bonus;
    // Armor Pieces can have other stats, e.g. +might etc. for now only armor
} equipable_item_t;

equipable_item_t* init_equipable_item(const char* name, gear_slot_t slot, int armor_bonus);

#endif //EQUIPABLE_ITEM_H
