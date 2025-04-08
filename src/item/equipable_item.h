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
    SLOT_LEFT_HAND, // Weapon slot
    SLOT_RIGHT_HAND, // Weapon slot
    SLOT_BOTH_HANDS, // Weapon slot
    MAX_SLOT
} gear_slot_t;

typedef struct {
    item_t* base; // base item structure
    gear_slot_t slot;
    int armor_bonus; // Armor Pieces can have other stats, e.g. +might etc. for now only armor
    // abilitiy_t abilities[6];
} equipable_item_t;

equipable_item_t* init_equipable_item(const char* name, gear_slot_t slot, int armor_bonus);
void free_equipable_item(equipable_item_t* item);

#endif //EQUIPABLE_ITEM_H
