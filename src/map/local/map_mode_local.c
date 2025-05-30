#include "map_mode_local.h"

#include "../../local/local_handler.h"
#include "../../common.h"

#include <stdlib.h>

char** map_mode_strings = NULL;

int init_map_mode_local(void) {
    map_mode_strings = malloc(sizeof(char*) * MAX_MAP_MODE_STRINGS);
    RETURN_WHEN_NULL(map_mode_strings, 1, "Map Mode Local", "Failed to allocate memory for map mode strings");
    for (int i = 0; i < MAX_MAP_MODE_STRINGS; i++) {
        map_mode_strings[i] = NULL;
    }

    update_map_mode_local();
    observe_local(update_map_mode_local);
    return 0;
}

void update_map_mode_local(void) {
    if (map_mode_strings == NULL) return; // module not initialized
    for (int i = 0; i < MAX_MAP_MODE_STRINGS; i++) {
        if (map_mode_strings[i] != NULL) {
            free(map_mode_strings[i]);
        }
    }

    map_mode_strings[PRESS_KEY_MENU] = get_local_string("MAP.PRESS.KEY.MENU");
    map_mode_strings[PRESS_KEY_STATS] = get_local_string("MAP.PRESS.KEY.STATS");
    map_mode_strings[PRESS_KEY_INVENTORY] = get_local_string("MAP.PRESS.KEY.INVENTORY");
    map_mode_strings[PLAYER_POSITION_STR] = get_local_string("MAP.PLAYER.POSITION");
}

void shutdown_map_mode_local(void) {
    if (map_mode_strings != NULL) {
        for (int i = 0; i < MAX_MAP_MODE_STRINGS; i++) {
            if (map_mode_strings[i] != NULL) {
                free(map_mode_strings[i]);
            }
        }
        free(map_mode_strings);
        map_mode_strings = NULL;
    }
}
