#include "loot_generation.h"
#include <stdlib.h>



void generate_loot(character_t* c, gear_table_t* gear_table, potion_table_t* potion_table, int rolls) {

    for (int i = 0; i < rolls; i++) {
        int gear_index = rand() % MAX_GEARS;
        int potion_index = rand() % MAX_POTION_TYPES;
        add_gear(c, gear_table->gears[gear_index]);
        add_potion(c, &potion_table->potions[potion_index]);
    }
}
