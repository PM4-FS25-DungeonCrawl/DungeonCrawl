#include "game.h"

#include "../include/termbox2.h"
#include "character_stats.h"
#include "combat_mode.h"
#include "logging/logger.h"
#include "map/map_generator.h"
#include "map/map_mode.h"
#include "local/local.h"

#include <stdbool.h>
#include <stdio.h>

enum game_state {
    MAIN_MENU,
    MAP_MODE,
    COMBAT_MODE,
    GENERATE_MAP,
    EXIT
};

int add(int a, int b) {
    return a + b;
}

int init_game() {
    if (tb_init() != 0) {
        log_msg(ERROR, "game", "Failed to initialize termbox");
        return 1;
    }
    tb_set_output_mode(TB_OUTPUT_NORMAL);

    init_map_mode();

    log_msg(INFO, "Game", "game loop starting");
    bool doRun = true;
    enum game_state currentState = GENERATE_MAP;

    while (doRun) {
        switch (currentState) {
            case MAIN_MENU:
                break;
            case GENERATE_MAP:
                generate_map();
                currentState = MAP_MODE;
                break;
            case MAP_MODE:
                switch (map_mode_update()) {
                    case CONTINUE:
                        break;
                    case QUIT:
                        currentState = EXIT;
                        break;
                    case NEXT_FLOOR:
                        currentState = GENERATE_MAP;
                        break;
                    default:
                        log_msg(ERROR, "game", "Unknown return value from map_mode_update");
                }
                break;
            case COMBAT_MODE: {
                // Initialize abilities
                ability_t fireball;
                initAbility(&fireball, "Fireball", 4, 10, D10, MAGICAL);
                ability_t swordslash;
                initAbility(&swordslash, "Swordslash", 4, 10, D6, PHYSICAL);
                ability_t bite;
                initAbility(&bite, "Bite", 3, 20, D8, PHYSICAL);

                // Initialize player
                player_t player;
                initCharacter(PLAYER, &player.base, "Hero", 100, 10, 5, 5, 5, 5);
                player.item_count = 0;//manually initializing player specific values
                for (int i = 0; i < MAX_ITEMS; i++) player.inventory[i] = NULL;
                addAbilityToCharacter(&player.base, fireball);
                addAbilityToCharacter(&player.base, swordslash);
                UsableItem healingPotion;
                init_usable_item(&healingPotion, "Healing Potion", HEALING, 20);
                add_item_to_player(&player, (Item*) &healingPotion);


                // Initialize monster
                monster_t monster;
                initCharacter(MONSTER, &monster.base, "Goblin", 50, 5, 3, 3, 3, 3);
                addAbilityToCharacter(&monster.base, bite);
                initWeaknesses(&monster, (int[]) {0, 10});

                currentState = (combat(&player, &monster)) ? MAP_MODE : EXIT;
                break;
            }
            case EXIT:
                doRun = false;
                break;
            default:
                break;
        }
    }

    shutdown_local();
    shutdown_logger();
    tb_shutdown();
    return 0;
}
