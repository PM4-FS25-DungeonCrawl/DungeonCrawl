#include "character_stats.h"

// Function to initialize a Character
void initCharacter(CharacterType type, Character *c, const char *name, int health, int armor, int accuracy, int might, int deflection, int fortitude, int will) {
    c->type = type;
    strncpy(c->name, name, sizeof(c->name) - 1);
    c->name[sizeof(c->name) - 1] = '\0';
    c->health = health;
    c->armor = armor;
    c->accuracy = accuracy;
    c->might = might;
    c->deflection = deflection;
    c->fortitude = fortitude;
    c->will = will;
    c->ability_count = 0;
}

// Function to initialize an Ability
void initAbility(Ability *a, const char *name, int rollCount, int accuracy, DiceSize diceSize, DamageType type) {
    strncpy(a->name, name, sizeof(a->name) - 1);
    a->name[sizeof(a->name) - 1] = '\0';
    a->rollCount = rollCount;
    a->accuracy = accuracy;
    a->diceSize = diceSize;
    a->damageType = type;
}

// Function to add an Ability to a Character
void addAbilityToCharacter(Character *c, Ability ability) {
    if (c->ability_count < MAX_ABILITIES) {
        c->abilities[c->ability_count++] = ability;
    } else {
        printf("Character %s cannot learn more abilities!\n", c->name);
    }
}

// Function to initialize the map containing all weaknesses
void initWeaknesses(Monster *m, int array[sizeof(Weakness)]) {
    for (int i = 0; i < sizeof(array); i++) {
        m->weakness_map[i].type = (DamageType)i;
        m->weakness_map[i].value = array[i];
    }
}

// Function to get the value corresponding to a damage type
int get_weakness_value(Monster *m, DamageType key) {
    for (int i = 0; i < sizeof(m->weakness_map); i++) {
        if (m->weakness_map[i].type == key){
            return m->weakness_map[i].value;
        }     
    }
    return 1;
}