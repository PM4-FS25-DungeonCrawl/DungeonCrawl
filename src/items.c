#include "items.h"

void init_usable_item(UsableItem *item, const char *name, UsableItemEffectType effectType, int value) {
    strncpy(item->base.name, name, sizeof(item->base.name) - 1);
    item->base.name[sizeof(item->base.name) - 1] = '\0';
    item->base.type = ITEM_TYPE_USABLE;
    item->effectType = effectType;
    item->value = value;
}

void init_equippable_item(EquippableItem *item, const char *name, GearSlot slot, int armor_bonus) {
    strncpy(item->base.name, name, sizeof(item->base.name) - 1);
    item->base.name[sizeof(item->base.name) - 1] = '\0';
    item->base.type = ITEM_TYPE_EQUIPPABLE;
    item->slot = slot;
    item->armor_bonus = armor_bonus;
}



