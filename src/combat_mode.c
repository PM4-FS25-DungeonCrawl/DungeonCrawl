#include "combat_mode.h"

/* TODO: Print lines for missed attacks and damage dealt
 * TODO: Critical hits
 * TODO: Enemy attacks after attacking
 * TODO: Might and weakness system
 */

bool combat(Player *player, Monster *monster) {

    // Set initial state of combat
    combat_state current_state = MENU_COMBAT;

    // Select menu state
    while(player->health > 0 && monster->health > 0) {
        switch (current_state)
        {
        case MENU_COMBAT:
            current_state = combat_menu(player, monster);
            break;
        case MENU_ABILITY:
            ability_menu(player, monster);
            current_state = MENU_COMBAT;
            break;
        case MENU_ITEM:
            item_menu(player, monster);
            current_state = MENU_COMBAT;
        default:
            break;
        }
    }

    // Return true if player is alive, false if they died
    return (player->health > 0);
}

combat_state combat_menu(Player *player, Monster *monster) {

    int selected_index = 0;
    const char *menu_options[] = {"Use Ability", "Use Item"};
    int menu_count = sizeof(menu_options) / sizeof(menu_options[0]);

    while (true) {

        // Prepare screen
        tb_clear();
        int y = print_combat_view(player, monster);

        // Display menu options
        tb_print(1, y++, TB_WHITE, TB_DEFAULT, "Menu:");

        for(int i = 0; i < menu_count; i++) {
            if (i == selected_index) {
                tb_print(1, y++, TB_BLACK, TB_WHITE, menu_options[i]);
            } else {
                tb_print(1, y++, TB_WHITE, TB_DEFAULT, menu_options[i]);
            }
        }

        // Print to terminal and check for key press
        tb_present();
        struct tb_event event;
        tb_poll_event(&event);

        if (event.type == TB_EVENT_KEY) {
            if (event.key == TB_KEY_ARROW_UP) {
                //Move up
                selected_index = (selected_index - 1 + menu_count) % menu_count;
            } else if (event.key == TB_KEY_ARROW_DOWN) {
                // Move down
                selected_index = (selected_index + 1) % menu_count;
            } else if (event.key == TB_KEY_ENTER) {
                // Return the selected state
                if (selected_index == 0) {
                    return MENU_ABILITY;
                } else if (selected_index == 1) {
                    return MENU_ITEM;
                }
            } else if (event.key == TB_KEY_CTRL_C) {
                // Exit the game
                tb_shutdown();
                exit(0);
            }
        }
    }
}

void ability_menu(Player *player, Monster *monster) {
    
    int selected_index = 0;
    int ability_count = player->abilityCount;

    while (true) {
        
        // Prepare screen
        tb_clear();
        int y = print_combat_view(player, monster);

        // Display menu options
        tb_print(1, y++, TB_WHITE, TB_DEFAULT, "Abilities:");
        for(int i = 0; i < ability_count; i++){
            if (i == selected_index) {
                tb_print(1, y++, TB_WHITE, TB_WHITE, player->abilities[i].name);
            } else {
                tb_print(1, y++, TB_WHITE, TB_DEFAULT, player->abilities[i].name);
            }
        }

        y++;
        tb_print(1, y++, TB_WHITE, TB_DEFAULT, "[ESC] Return to menu");

        // Print to terminal and check for key press
        tb_present();
        struct tb_event event;
        tb_poll_event(&event);

        // Do key press event
        if (event.type == TB_EVENT_KEY) {
            if (event.key == TB_KEY_ARROW_UP) {
                // Move up
                selected_index = (selected_index - 1 + ability_count) % ability_count;
            } else if (event.key == TB_KEY_ARROW_DOWN) {
                // Move down
                selected_index = (selected_index + 1) % ability_count;
            } else if (event.key == TB_KEY_ENTER) {
                // Use the selected ability
                use_ability(player, monster, &player->abilities[selected_index]);
                break; // Exit the menu after using the ability
            } else if (event.key == TB_KEY_ESC) {
                // Go back to the combat menu
                break;
            }
        }
    }
}

void item_menu(Player *player, Monster *monster) {
    /* TODO */
}

void use_ability(Player *player, Monster *monster, Ability *ability) {
    // Roll to hit
    if (roll_hit(player, ability, monster)) {
        // Roll damage
        int damage = roll_damage(ability);
        deal_damage(damage, monster);
    }
}

bool roll_hit(Player *player, Ability *ability, Monster *monster) {
    int roll = roll_dice(D20);
    switch (ability->damageType) {
        case PHYSICAL:
            return roll + ability->accuracy > monster->deflection;
        case MAGICAL:
            return roll + ability->accuracy > monster->fortitude;
    }
    return false;
}

int roll_damage(Ability *ability) {
    int roll = 0;
    // Roll the dice several times
    for (int i = 0; i < ability->rollCount; i++){
        roll += roll_dice(ability->diceSize);
    }
    return roll;
}

void deal_damage(int damage, Monster *monster) {
    /* TODO critical hits are ignored */
    damage -= monster->armor;
    if (damage > 0) monster->health -= damage;
}

void take_damage(Monster *monster, Player *player) {
    /* TODO */
}

void use_item() {
    /* TODO */
}

int print_combat_view(Player *player, Monster *monster){

    int y = 1;

    // Display player info
    char player_info[100];
    snprintf(player_info, sizeof(player_info), "Player: %s | Health %d", player->name, player->health);
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, player_info);

    // Display monster info
    char monster_info[100];
    snprintf(monster_info, sizeof(monster_info), "Monster: %s | Health %d", monster->name, monster->health);
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, monster_info);

    y += 5;
    for (int i = 0; i < 20; i++) {
        tb_printf(1, y, TB_WHITE, TB_DEFAULT, "_");
    }

    return y;
}

int roll_dice(DiceSize dice_size) {
    /* TODO better randomness? (warning message)*/
    return rand() % dice_size + 1;
}