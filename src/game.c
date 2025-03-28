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
                Ability fireball;
                initAbility(&fireball, "Fireball", 4, 10, D10, MAGICAL);
                Ability swordslash;
                initAbility(&swordslash, "Swordslash", 4, 10, D6, PHYSICAL);
                Ability bite;
                initAbility(&bite, "Bite", 3, 20, D8, PHYSICAL);

                // Initialize usable Items
                UsableItem healingPotion;
                initUsableItem(&healingPotion, "Healing Potion", HEALING, 30);
                UsableItem armorPotion;
                initUsableItem(&armorPotion, "Stoneshield Potion", ARMOR_INCREASE, 5);

                // Initialize player
                Player player;
                initCharacter(PLAYER, &player.base, "Hero", 100, 10, 5, 5, 5, 5, 5);
                addAbilityToCharacter(&player.base, fireball);
                addAbilityToCharacter(&player.base, swordslash);
                addItemToPlayer(&player.base, healingPotion);
                addItemToPlayer(&player.base, armorPotion);

                // Initialize monster
                Monster monster;
                initCharacter(MONSTER, &monster.base, "Goblin", 50, 5, 3, 3, 3, 3, 3);
                addAbilityToCharacter(&monster.base, bite);
                initWeaknesses(&monster, (int[]){0,10});

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
