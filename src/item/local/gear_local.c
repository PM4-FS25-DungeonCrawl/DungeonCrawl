#include "gear_local.h"

#include "../../local/local_handler.h"
#include "../../logging/logger.h"
#include "../gear.h"

char** gear_names = NULL;

void update_gear_local(void);

int init_gear_local(void) {
    gear_names = malloc(sizeof(char*) * MAX_GEARS);
    RETURN_WHEN_NULL(gear_names, 1, "Gear Local", "Failed to allocate memory for gear names");
    for (int i = 0; i < MAX_GEARS; i++) {
        gear_names[i] = NULL;
    }

    update_gear_local();
    observe_local(update_gear_local);
    return 0;
}

void shutdown_gear_local(void) {
    if (gear_names != NULL) {
        for (int i = 0; i < MAX_GEARS; i++) {
            if (gear_names[i] != NULL) {
                free(gear_names[i]);
            }
        }
        free(gear_names);
        gear_names = NULL;
    }
}

void update_gear_local(void) {
    if (gear_names == NULL) return; // module not initialized

    // needs to free up the old strings until to gear: TRICKSTERS_LOOP_OF_THE_RAVEN_R
    for (int i = 0; i < IRON_BAND_OF_THE_GOLIATH_L; i++) {
        if (gear_names[i] != NULL) {
            free(gear_names[i]);
        }
    }

    gear_names[LONGSWORD] = get_local_string("GEAR.LONGSWORD");
    gear_names[BARDICHE] = get_local_string("GEAR.BARDICHE");
    gear_names[CROSSBOW] = get_local_string("GEAR.CROSSBOW");
    gear_names[MAGIC_STAFF] = get_local_string("GEAR.MAGIC_STAFF");
    gear_names[SHORT_AXE] = get_local_string("GEAR.SHORT_AXE");
    gear_names[STILETTO_DAGGER] = get_local_string("GEAR.STILETTO_DAGGER");
    gear_names[BUCKLER] = get_local_string("GEAR.BUCKLER");
    gear_names[MACE] = get_local_string("GEAR.MACE");
    gear_names[WAND] = get_local_string("GEAR.WAND");
    gear_names[ARMING_SWORD] = get_local_string("GEAR.ARMING_SWORD");
    gear_names[IRON_HELM_OF_THE_GOLIATH] = get_local_string("GEAR.IRON_HELM_OF_THE_GOLIATH");
    gear_names[IRON_HELM_OF_THE_BOAR] = get_local_string("GEAR.IRON_HELM_OF_THE_BOAR");
    gear_names[IRON_HELM_OF_THE_BEAR] = get_local_string("GEAR.IRON_HELM_OF_THE_BEAR");
    gear_names[SCOUTS_HOOD_OF_THE_BOAR] = get_local_string("GEAR.SCOUTS_HOOD_OF_THE_BOAR");
    gear_names[SCOUTS_HOOD_OF_THE_FOX] = get_local_string("GEAR.SCOUTS_HOOD_OF_THE_FOX");
    gear_names[SCOUTS_HOOD_OF_THE_OWL] = get_local_string("GEAR.SCOUTS_HOOD_OF_THE_OWL");
    gear_names[SCOUTS_HOOD_OF_THE_RAVEN] = get_local_string("GEAR.SCOUTS_HOOD_OF_THE_RAVEN");
    gear_names[SCOUTS_HOOD_OF_THE_WOLF] = get_local_string("GEAR.SCOUTS_HOOD_OF_THE_WOLF");
    gear_names[SCOUTS_HOOD_OF_THE_LICH] = get_local_string("GEAR.SCOUTS_HOOD_OF_THE_LICH");
    gear_names[SHADOW_HOOD_OF_THE_FOX] = get_local_string("GEAR.SHADOW_HOOD_OF_THE_FOX");
    gear_names[SHADOW_HOOD_OF_THE_OWL] = get_local_string("GEAR.SHADOW_HOOD_OF_THE_OWL");
    gear_names[SHADOW_HOOD_OF_THE_RAVEN] = get_local_string("GEAR.SHADOW_HOOD_OF_THE_RAVEN");
    gear_names[BATTLEPLATE_OF_THE_GOLIATH] = get_local_string("GEAR.BATTLEPLATE_OF_THE_GOLIATH");
    gear_names[BATTLEPLATE_OF_THE_BOAR] = get_local_string("GEAR.BATTLEPLATE_OF_THE_BOAR");
    gear_names[BATTLEPLATE_OF_THE_BEAR] = get_local_string("GEAR.BATTLEPLATE_OF_THE_BEAR");
    gear_names[REINFORCED_JERKIN_OF_THE_BOAR] = get_local_string("GEAR.REINFORCED_JERKIN_OF_THE_BOAR");
    gear_names[REINFORCED_JERKIN_OF_THE_FOX] = get_local_string("GEAR.REINFORCED_JERKIN_OF_THE_FOX");
    gear_names[REINFORCED_JERKIN_OF_THE_OWL] = get_local_string("GEAR.REINFORCED_JERKIN_OF_THE_OWL");
    gear_names[REINFORCED_JERKIN_OF_THE_RAVEN] = get_local_string("GEAR.REINFORCED_JERKIN_OF_THE_RAVEN");
    gear_names[REINFORCED_JERKIN_OF_THE_WOLF] = get_local_string("GEAR.REINFORCED_JERKIN_OF_THE_WOLF");
    gear_names[REINFORCED_JERKIN_OF_THE_LICH] = get_local_string("GEAR.REINFORCED_JERKIN_OF_THE_LICH");
    gear_names[LEATHER_JERKIN_OF_THE_FOX] = get_local_string("GEAR.LEATHER_JERKIN_OF_THE_FOX");
    gear_names[LEATHER_JERKIN_OF_THE_OWL] = get_local_string("GEAR.LEATHER_JERKIN_OF_THE_OWL");
    gear_names[LEATHER_JERKIN_OF_THE_RAVEN] = get_local_string("GEAR.LEATHER_JERKIN_OF_THE_RAVEN");
    gear_names[PLATED_GREAVES_OF_THE_GOLIATH] = get_local_string("GEAR.PLATED_GREAVES_OF_THE_GOLIATH");
    gear_names[PLATED_GREAVES_OF_THE_BOAR] = get_local_string("GEAR.PLATED_GREAVES_OF_THE_BOAR");
    gear_names[PLATED_GREAVES_OF_THE_BEAR] = get_local_string("GEAR.PLATED_GREAVES_OF_THE_BEAR");
    gear_names[RANGERS_TROUSERS_OF_THE_BOAR] = get_local_string("GEAR.RANGERS_TROUSERS_OF_THE_BOAR");
    gear_names[RANGERS_TROUSERS_OF_THE_FOX] = get_local_string("GEAR.RANGERS_TROUSERS_OF_THE_FOX");
    gear_names[RANGERS_TROUSERS_OF_THE_OWL] = get_local_string("GEAR.RANGERS_TROUSERS_OF_THE_OWL");
    gear_names[RANGERS_TROUSERS_OF_THE_RAVEN] = get_local_string("GEAR.RANGERS_TROUSERS_OF_THE_RAVEN");
    gear_names[RANGERS_TROUSERS_OF_THE_WOLF] = get_local_string("GEAR.RANGERS_TROUSERS_OF_THE_WOLF");
    gear_names[RANGERS_TROUSERS_OF_THE_LICH] = get_local_string("GEAR.RANGERS_TROUSERS_OF_THE_LICH");
    gear_names[SILENT_LEGGINGS_OF_THE_FOX] = get_local_string("GEAR.SILENT_LEGGINGS_OF_THE_FOX");
    gear_names[SILENT_LEGGINGS_OF_THE_OWL] = get_local_string("GEAR.SILENT_LEGGINGS_OF_THE_OWL");
    gear_names[SILENT_LEGGINGS_OF_THE_RAVEN] = get_local_string("GEAR.SILENT_LEGGINGS_OF_THE_RAVEN");
    gear_names[STEEL_SABATONS_OF_THE_GOLIATH] = get_local_string("GEAR.STEEL_SABATONS_OF_THE_GOLIATH");
    gear_names[STEEL_SABATONS_OF_THE_BOAR] = get_local_string("GEAR.STEEL_SABATONS_OF_THE_BOAR");
    gear_names[STEEL_SABATONS_OF_THE_BEAR] = get_local_string("GEAR.STEEL_SABATONS_OF_THE_BEAR");
    gear_names[TRAVELERS_BOOTS_OF_THE_BOAR] = get_local_string("GEAR.TRAVELERS_BOOTS_OF_THE_BOAR");
    gear_names[TRAVELERS_BOOTS_OF_THE_FOX] = get_local_string("GEAR.TRAVELERS_BOOTS_OF_THE_FOX");
    gear_names[TRAVELERS_BOOTS_OF_THE_OWL] = get_local_string("GEAR.TRAVELERS_BOOTS_OF_THE_OWL");
    gear_names[TRAVELERS_BOOTS_OF_THE_RAVEN] = get_local_string("GEAR.TRAVELERS_BOOTS_OF_THE_RAVEN");
    gear_names[TRAVELERS_BOOTS_OF_THE_WOLF] = get_local_string("GEAR.TRAVELERS_BOOTS_OF_THE_WOLF");
    gear_names[TRAVELERS_BOOTS_OF_THE_LICH] = get_local_string("GEAR.TRAVELERS_BOOTS_OF_THE_LICH");
    gear_names[WINDSTEP_BOOTS_OF_THE_FOX] = get_local_string("GEAR.WINDSTEP_BOOTS_OF_THE_FOX");
    gear_names[WINDSTEP_BOOTS_OF_THE_OWL] = get_local_string("GEAR.WINDSTEP_BOOTS_OF_THE_OWL");
    gear_names[WINDSTEP_BOOTS_OF_THE_RAVEN] = get_local_string("GEAR.WINDSTEP_BOOTS_OF_THE_RAVEN");
    gear_names[GAUNTLETS_OF_THE_GOLIATH] = get_local_string("GEAR.GAUNTLETS_OF_THE_GOLIATH");
    gear_names[GAUNTLETS_OF_THE_BOAR] = get_local_string("GEAR.GAUNTLETS_OF_THE_BOAR");
    gear_names[GAUNTLETS_OF_THE_BEAR] = get_local_string("GEAR.GAUNTLETS_OF_THE_BEAR");
    gear_names[LEATHER_GLOVES_OF_THE_BOAR] = get_local_string("GEAR.LEATHER_GLOVES_OF_THE_BOAR");
    gear_names[LEATHER_GLOVES_OF_THE_FOX] = get_local_string("GEAR.LEATHER_GLOVES_OF_THE_FOX");
    gear_names[LEATHER_GLOVES_OF_THE_OWL] = get_local_string("GEAR.LEATHER_GLOVES_OF_THE_OWL");
    gear_names[LEATHER_GLOVES_OF_THE_RAVEN] = get_local_string("GEAR.LEATHER_GLOVES_OF_THE_RAVEN");
    gear_names[LEATHER_GLOVES_OF_THE_WOLF] = get_local_string("GEAR.LEATHER_GLOVES_OF_THE_WOLF");
    gear_names[LEATHER_GLOVES_OF_THE_LICH] = get_local_string("GEAR.LEATHER_GLOVES_OF_THE_LICH");
    gear_names[NIMBLE_GRIPS_OF_THE_FOX] = get_local_string("GEAR.NIMBLE_GRIPS_OF_THE_FOX");
    gear_names[NIMBLE_GRIPS_OF_THE_OWL] = get_local_string("GEAR.NIMBLE_GRIPS_OF_THE_OWL");
    gear_names[NIMBLE_GRIPS_OF_THE_RAVEN] = get_local_string("GEAR.NIMBLE_GRIPS_OF_THE_RAVEN");
    gear_names[IRON_GORGET_OF_THE_GOLIATH] = get_local_string("GEAR.IRON_GORGET_OF_THE_GOLIATH");
    gear_names[IRON_GORGET_OF_THE_BOAR] = get_local_string("GEAR.IRON_GORGET_OF_THE_BOAR");
    gear_names[IRON_GORGET_OF_THE_BEAR] = get_local_string("GEAR.IRON_GORGET_OF_THE_BEAR");
    gear_names[PENDANT_OF_FOCUS_OF_THE_BOAR] = get_local_string("GEAR.PENDANT_OF_FOCUS_OF_THE_BOAR");
    gear_names[PENDANT_OF_FOCUS_OF_THE_FOX] = get_local_string("GEAR.PENDANT_OF_FOCUS_OF_THE_FOX");
    gear_names[PENDANT_OF_FOCUS_OF_THE_OWL] = get_local_string("GEAR.PENDANT_OF_FOCUS_OF_THE_OWL");
    gear_names[PENDANT_OF_FOCUS_OF_THE_RAVEN] = get_local_string("GEAR.PENDANT_OF_FOCUS_OF_THE_RAVEN");
    gear_names[PENDANT_OF_FOCUS_OF_THE_WOLF] = get_local_string("GEAR.PENDANT_OF_FOCUS_OF_THE_WOLF");
    gear_names[PENDANT_OF_FOCUS_OF_THE_LICH] = get_local_string("GEAR.PENDANT_OF_FOCUS_OF_THE_LICH");
    gear_names[WHISPER_CHARM_OF_THE_FOX] = get_local_string("GEAR.WHISPER_CHARM_OF_THE_FOX");
    gear_names[WHISPER_CHARM_OF_THE_OWL] = get_local_string("GEAR.WHISPER_CHARM_OF_THE_OWL");
    gear_names[WHISPER_CHARM_OF_THE_RAVEN] = get_local_string("GEAR.WHISPER_CHARM_OF_THE_RAVEN");
    gear_names[IRON_BAND_OF_THE_GOLIATH_R] = get_local_string("GEAR.IRON_BAND_OF_THE_GOLIATH");
    gear_names[IRON_BAND_OF_THE_BOAR_R] = get_local_string("GEAR.IRON_BAND_OF_THE_BOAR");
    gear_names[IRON_BAND_OF_THE_BEAR_R] = get_local_string("GEAR.IRON_BAND_OF_THE_BEAR");
    gear_names[CARVED_RING_OF_THE_BOAR_R] = get_local_string("GEAR.CARVED_RING_OF_THE_BOAR");
    gear_names[CARVED_RING_OF_THE_FOX_R] = get_local_string("GEAR.CARVED_RING_OF_THE_FOX");
    gear_names[CARVED_RING_OF_THE_OWL_R] = get_local_string("GEAR.CARVED_RING_OF_THE_OWL");
    gear_names[CARVED_RING_OF_THE_RAVEN_R] = get_local_string("GEAR.CARVED_RING_OF_THE_RAVEN");
    gear_names[CARVED_RING_OF_THE_WOLF_R] = get_local_string("GEAR.CARVED_RING_OF_THE_WOLF");
    gear_names[CARVED_RING_OF_THE_LICH_R] = get_local_string("GEAR.CARVED_RING_OF_THE_LICH");
    gear_names[TRICKSTERS_LOOP_OF_THE_FOX_R] = get_local_string("GEAR.TRICKSTERS_LOOP_OF_THE_FOX");
    gear_names[TRICKSTERS_LOOP_OF_THE_OWL_R] = get_local_string("GEAR.TRICKSTERS_LOOP_OF_THE_OWL");
    gear_names[TRICKSTERS_LOOP_OF_THE_RAVEN_R] = get_local_string("GEAR.TRICKSTERS_LOOP_OF_THE_RAVEN");

    // Left-hand side gear names are the same as right-hand side, so we can just copy them
    gear_names[IRON_BAND_OF_THE_GOLIATH_L] = gear_names[IRON_BAND_OF_THE_GOLIATH_R];
    gear_names[IRON_BAND_OF_THE_BOAR_L] = gear_names[IRON_BAND_OF_THE_BOAR_R];
    gear_names[IRON_BAND_OF_THE_BEAR_L] = gear_names[IRON_BAND_OF_THE_BEAR_R];
    gear_names[CARVED_RING_OF_THE_BOAR_L] = gear_names[CARVED_RING_OF_THE_BOAR_R];
    gear_names[CARVED_RING_OF_THE_FOX_L] = gear_names[CARVED_RING_OF_THE_FOX_R];
    gear_names[CARVED_RING_OF_THE_OWL_L] = gear_names[CARVED_RING_OF_THE_OWL_R];
    gear_names[CARVED_RING_OF_THE_RAVEN_L] = gear_names[CARVED_RING_OF_THE_RAVEN_R];
    gear_names[CARVED_RING_OF_THE_WOLF_L] = gear_names[CARVED_RING_OF_THE_WOLF_R];
    gear_names[CARVED_RING_OF_THE_LICH_L] = gear_names[CARVED_RING_OF_THE_LICH_R];
    gear_names[TRICKSTERS_LOOP_OF_THE_FOX_L] = gear_names[TRICKSTERS_LOOP_OF_THE_FOX_R];
    gear_names[TRICKSTERS_LOOP_OF_THE_OWL_L] = gear_names[TRICKSTERS_LOOP_OF_THE_OWL_R];
    gear_names[TRICKSTERS_LOOP_OF_THE_RAVEN_L] = gear_names[TRICKSTERS_LOOP_OF_THE_RAVEN_R];
}
