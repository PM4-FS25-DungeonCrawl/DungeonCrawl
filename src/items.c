#include "items.h"

void init_usable_item(usable_item_t *item, const char *name, usable_item_effect_t effectType, int value) {
    strncpy(item->base.name, name, sizeof(item->base.name) - 1);
    item->base.name[sizeof(item->base.name) - 1] = '\0';
    item->base.type = ITEM_TYPE_USABLE;
    item->effectType = effectType;
    item->value = value;
}

void init_equipable_item(equipable_item_t *item, const char *name, gear_slot_t slot, int armor_bonus) {
    strncpy(item->base.name, name, sizeof(item->base.name) - 1);
    item->base.name[sizeof(item->base.name) - 1] = '\0';
    item->base.type = ITEM_TYPE_EQUIPPABLE;
    item->slot = slot;
    item->armor_bonus = armor_bonus;
}
