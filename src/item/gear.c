#include "gear.h"

#include <stdio.h>
#include <stdlib.h>

gear_t* init_gear(const char* name, gear_slot_t slot, stats_t stats, defenses_t defenses) {
    gear_t* gear = malloc(sizeof(gear_t));
    NULL_PTR_HANDLER_RETURN(gear, NULL, "Gear", "Failed to allocate memory for gear: %s", name);

    snprintf(gear->name, sizeof(gear->name), "%s", name);
    gear->slot = slot;
    gear->stats = stats;
    gear->defenses = defenses;
    return gear;
}

void free_equipable_gear(gear_t* gear) {
    if (gear != NULL) {
        free(gear);
    }
}

const char* gear_slot_to_string(gear_slot_t slot) {
    switch (slot) {
        case SLOT_HEAD: return "Head";
        case SLOT_CHEST: return "Chest";
        case SLOT_LEGS: return "Legs";
        case SLOT_FEET: return "Feet";
        case SLOT_HANDS: return "Hands";
        case SLOT_NECK: return "Neck";
        case SLOT_FINGER_RIGHT: return "Right Finger";
        case SLOT_FINGER_LEFT: return "Left Finger";
        case SLOT_LEFT_HAND: return "Left Hand";
        case SLOT_RIGHT_HAND: return "Right Hand";
        case SLOT_BOTH_HANDS: return "Both Hands";
        default: return "Unknown Slot";
    }
}
