#include "ability_local.h"

#include "../../local/local_handler.h"
#include "../../logging/logger.h"
#include "../ability.h"

char** ability_names = NULL;

void update_ability_local(void);

int init_ability_local(void) {
    ability_names = malloc(sizeof(char*) * MAX_ABILITIES);
    RETURN_WHEN_NULL(ability_names, 1, "Ability Local", "Failed to allocate memory for ability names");
    for (int i = 0; i < MAX_ABILITIES; i++) {
        ability_names[i] = NULL;
    }

    update_ability_local();
    observe_local(update_ability_local);
    return 0;
}

void shutdown_ability_local(void) {
    if (ability_names != NULL) {
        for (int i = 0; i < MAX_ABILITIES; i++) {
            if (ability_names[i] != NULL) {
                free(ability_names[i]);
                ability_names[i] = NULL;
            }
        }
        free(ability_names);
        ability_names = NULL;
    }
}

void update_ability_local(void) {
    if (ability_names == NULL) return;// module not initialized

    for (int i = 0; i < MAX_ABILITIES; i++) {
        if (ability_names[i] != NULL) {
            free(ability_names[i]);
        }
    }

    ability_names[BITE] = get_local_string("ABILITY.BITE");
    ability_names[QUICK_SLASH] = get_local_string("ABILITY.QUICK_SLASH");
    ability_names[HEAVY_SWING] = get_local_string("ABILITY.HEAVY_SWING");
    ability_names[SWEEPING_STRIKE] = get_local_string("ABILITY.SWEEPING_STRIKE");
    ability_names[GUARDING_STANCE] = get_local_string("ABILITY.GUARDING_STANCE");
    ability_names[HEAVY_CHOP] = get_local_string("ABILITY.HEAVY_CHOP");
    ability_names[PIERCING_STRIKE] = get_local_string("ABILITY.PIERCING_STRIKE");
    ability_names[EXECUTE] = get_local_string("ABILITY.EXECUTE");
    ability_names[BERSERKER_RAGE] = get_local_string("ABILITY.BERSERKER_RAGE");
    ability_names[QUICK_SHOT] = get_local_string("ABILITY.QUICK_SHOT");
    ability_names[POWER_SHOT] = get_local_string("ABILITY.POWER_SHOT");
    ability_names[STEADY_SHOT] = get_local_string("ABILITY.STEADY_SHOT");
    ability_names[STEADY_AIM] = get_local_string("ABILITY.STEADY_AIM");
    ability_names[FIREBLAST] = get_local_string("ABILITY.FIREBLAST");
    ability_names[FIREBALL] = get_local_string("ABILITY.FIREBALL");
    ability_names[PYROBLAST] = get_local_string("ABILITY.PYROBLAST");
    ability_names[MANA_SHIELD] = get_local_string("ABILITY.MANA_SHIELD");
    ability_names[CHOP] = get_local_string("ABILITY.CHOP");
    ability_names[AXE_SWING] = get_local_string("ABILITY.AXE_SWING");
    ability_names[BACKSTAB] = get_local_string("ABILITY.BACKSTAB");
    ability_names[SINISTER_STRIKE] = get_local_string("ABILITY.SINISTER_STRIKE");
    ability_names[DEFLECT] = get_local_string("ABILITY.DEFLECT");
    ability_names[SHIELD_WALL] = get_local_string("ABILITY.SHIELD_WALL");
    ability_names[MACE_STRIKE] = get_local_string("ABILITY.MACE_STRIKE");
    ability_names[CRUSHING_BLOW] = get_local_string("ABILITY.CRUSHING_BLOW");
    ability_names[ARCANE_BOLT] = get_local_string("ABILITY.ARCANE_BOLT");
    ability_names[ARCANE_MISSILE] = get_local_string("ABILITY.ARCANE_MISSILE");
    ability_names[SWORD_SLASH] = get_local_string("ABILITY.SWORD_SLASH");
    ability_names[RIPOSTE] = get_local_string("ABILITY.RIPOSTE");
    ability_names[PUNCH] = get_local_string("ABILITY.PUNCH");
}
