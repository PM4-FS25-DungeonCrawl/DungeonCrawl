#include <stdio.h>
#include <stdbool.h>
#include "game.h"
#include "map_mode.h"
#include "combat_mode.h"
#include "character_stats.h"
#include "../include/termbox2.h"

enum game_state {
    MAIN_MENU,
    MAP_MODE,
    COMBAT_MODE,
    EXIT
};

int add(int a, int b) {
    return a + b;
}

int init_game(){
    if (tb_init()!= 0) {
        fprintf(stderr, "Failed to initialize termbox \n");
        return 1;
    }
    tb_set_output_mode(TB_OUTPUT_NORMAL);

    bool doRun = true;
    enum game_state currentState = COMBAT_MODE;

    while (doRun) {
        switch (currentState) {
            case MAIN_MENU:
                break;
            case MAP_MODE:
                if (mapModeUpdate()) {
                    currentState = EXIT;
                }
                break;
            case COMBAT_MODE:
                // Initialize abilities
                Ability basicAttack;
                initAbility(&basicAttack, "Basic Attack", 10, PHYSICAL);
                Ability fireball;
                initAbility(&fireball, "Fireball", 20, MAGICAL);
                Ability swordslash;
                initAbility(&swordslash, "Swordslash", 20, PHYSICAL);

                // Initialize player
                Player player;
                initPlayer(&player, "Hero", 100, 10, 5, 5, 5, 5, 5, basicAttack);

                addAbilityToPlayer(&player, fireball);
                addAbilityToPlayer(&player, swordslash);

                // Initialize monster
                Monster monster;
                initMonster(&monster, "Goblin", 50, 5, 3, 3, 3, 3, 3, basicAttack);

                currentState = (combat(&player, &monster))? COMBAT_MODE : EXIT;
                break;
            case EXIT:
                doRun = false;
            default:
        }
    }

    tb_shutdown();
    return 0;
}
