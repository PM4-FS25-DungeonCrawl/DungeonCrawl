#ifndef GEAR_H
#define GEAR_H

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
    char name[MAX_NAME_LENGTH];
    gear_slot_t slot;
    stats_t stats;
    defenses_t defenses; // Armor Pieces can have other stats, e.g. +might etc. for now only armor
    // abilitiy_t abilities[6];
} gear_t;

gear_t* init_gear(const char* name, gear_slot_t slot, stats_t stats, defenses_t defenses);
void free_equipable_gear(gear_t* gear);

#endif //GEAR_H
