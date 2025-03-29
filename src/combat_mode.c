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
    while(player->base.health > 0 && monster->base.health > 0) {
        switch (current_state) {
        case MENU_COMBAT:
            current_state = combat_menu(player, monster);
            break;
        case MENU_ABILITY:
            ability_menu(player, monster);
            current_state = MENU_COMBAT;
            use_ability(&monster->base, &player->base, get_random_ability(&monster->base));
            break;
        case MENU_ITEM:
            item_menu(player, monster);
            current_state = MENU_COMBAT;
            use_ability(&monster->base, &player->base, get_random_ability(&monster->base));
        default:
            break;
        }
        
    }

    // Return true if player is alive, false if they died
    return (player->base.health > 0);
}

combat_state combat_menu(Player *player, Monster *monster) {

    int selected_index = 0;
    const char *menu_options[] = {"Use Ability", "Use Item"};
    int menu_count = sizeof(menu_options) / sizeof(menu_options[0]);

    while (true) {

        // Prepare screen
        tb_clear();
        int y = print_combat_view(&player->base, &monster->base);

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
    int ability_count = player->base.ability_count;

    while (true) {
        
        // Prepare screen
        tb_clear();
        int y = print_combat_view(&player->base, &monster->base);

        // Display menu options
        tb_print(1, y++, TB_WHITE, TB_DEFAULT, "Abilities:");
        for(int i = 0; i < ability_count; i++){
            if (i == selected_index) {
                tb_print(1, y++, TB_WHITE, TB_WHITE, player->base.abilities[i].name);
            } else {
                tb_print(1, y++, TB_WHITE, TB_DEFAULT, player->base.abilities[i].name);
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
                use_ability(&player->base, &monster->base, &player->base.abilities[selected_index]);
                break; // Exit the menu after using the ability
            } else if (event.key == TB_KEY_ESC) {
                // Go back to the combat menu
                break;
            }
        }
    }
}

void item_menu(Player *player, Monster *monster) {

    int selected_index = 0;
    int usable_item_count = 0;
    UsableItem *usable_items[MAX_ITEMS];

    // Collect all usable items
    for (int i = 0; i < player->item_count; i++) {
        if (player->inventory[i]->type == ITEM_TYPE_USABLE) {
            usable_items[usable_item_count++] = (UsableItem *)player->inventory[i];
        }
    }

    while (true) {

        // Prepare screen
        tb_clear();
        int y = print_combat_view(&player->base, &monster->base);

        // Display menu options
        tb_print(1, y++, TB_WHITE, TB_DEFAULT, "Usable Items:");
        for (int i = 0; i < usable_item_count; i++) {
            if (i == selected_index) {
                tb_print(1, y++, TB_BLACK, TB_WHITE, usable_items[i]->base.name);
            } else {
                tb_print(1, y++, TB_WHITE, TB_DEFAULT, usable_items[i]->base.name);
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
                selected_index = (selected_index - 1 + usable_item_count) % usable_item_count;
            } else if (event.key == TB_KEY_ARROW_DOWN) {
                // Move down
                selected_index = (selected_index + 1) % usable_item_count;
            } else if (event.key == TB_KEY_ENTER) {
                // Use the selected item
                use_item(player, usable_items[selected_index]);
                break; // Exit the menu after using the item
            } else if (event.key == TB_KEY_ESC) {
                // Go back to the combat menu
                break;
            }
        }
    }
}

void use_ability(Character *attacker, Character *defender, Ability *ability) {
    // Roll to hit
    if (roll_hit(ability, defender)) {
        // Roll damage
        int damage = roll_damage(ability);
        deal_damage(damage, ability->damageType, defender);
    }
}

bool roll_hit(Ability *ability, Character *defender) {
    int roll = roll_dice(D20);
    switch (ability->damageType) {
        case PHYSICAL:
            return roll + ability->accuracy > defender->deflection;
        case MAGICAL:
            return roll + ability->accuracy > defender->fortitude;
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

void deal_damage(int damage, DamageType damage_type, Character *character) {
    /* TODO critical hits are ignored */
    if (character->type == MONSTER) {
        Monster *monster = (Monster *)character;
        damage += get_weakness_value(monster, damage_type);
    }
    damage -= character->armor;
    if (damage > 0) character->health -= damage;
}

void use_item(Player *player, UsableItem *item) {
    switch (item->effectType) {
    case HEALING:
        player->base.health += item->value;
        break;
    case ARMOR_INCREASE:
        player->base.armor += item->value;
        break;
    default:
        break;
    }

    for (int i = 0; i < player->item_count; i++) {
        if (player->inventory[i] == (Item *)item) {
            for (int j = i; j < player->item_count - 1; j++) {
                player->inventory[j] = player->inventory[j + 1];
            }
            player->inventory[player->item_count - 1] = NULL;
            player->item_count--;
            break;
        }
    }
}

int print_combat_view(Character *player, Character *monster){

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

Ability *get_random_ability(Character *character){
    int random_index = rand() % character->ability_count;
    return &character->abilities[random_index];
}
