#include "character_stats.h"

// Function to initialize a Character
void initCharacter(character_type_t type, character_t* c, const char* name, int health, int armor, int might, int deflection, int fortitude, int will) {
    c->type = type;
    strncpy(c->name, name, sizeof(c->name) - 1);
    c->name[sizeof(c->name) - 1] = '\0';
    c->health = health;
    c->armor = armor;
    c->might = might;
    c->deflection = deflection;
    c->fortitude = fortitude;
    c->will = will;
    c->ability_count = 0;
}

// Function to initialize an Ability
void initAbility(ability_t* a, const char* name, int rollCount, int accuracy, dice_size_t diceSize, damage_type_t type) {
    strncpy(a->name, name, sizeof(a->name) - 1);
    a->name[sizeof(a->name) - 1] = '\0';
    a->rollCount = rollCount;
    a->accuracy = accuracy;
    a->diceSize = diceSize;
    a->damageType = type;
}

// Function to add an Ability to a Character
void addAbilityToCharacter(character_t* c, ability_t ability) {
    if (c->ability_count < MAX_ABILITIES) {
        c->abilities[c->ability_count++] = ability;
    } else {
        printf("Character %s cannot learn more abilities!\n", c->name);
    }
}

// Function to initialize the map containing all weaknesses
void initWeaknesses(monster_t* m, int array[]) {
    for (int i = 0; i < DAMAGE_TYPE_COUNT; i++) {
        m->weakness_map[i].type = (damage_type_t) i;
        m->weakness_map[i].value = array[i];
    }
}

// Function to get the value corresponding to a damage type
int get_weakness_value(monster_t* m, damage_type_t key) {
    for (int i = 0; i < sizeof(m->weakness_map); i++) {
        if (m->weakness_map[i].type == key) {
            return m->weakness_map[i].value;
        }
    }
    return 1;
}

void add_item_to_player(player_t* p, Item* item) {
    if (p->item_count < MAX_ITEMS) {
        p->inventory[p->item_count++] = item;
    } else {
        printf("Item Inventory is full!\n");
    }
}
