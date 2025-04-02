#include "combat_mode.h"
#include "../include/termbox2.h"
/* TODO: Print lines for missed attacks and damage dealt
 * TODO: Critical hits
 * TODO: Enemy attacks after attacking
 * TODO: Might and weakness system
 */

bool combat(player_t *player, monster_t *monster) {

    // Set initial state of combat
    combat_state_t current_state = MENU_COMBAT;

    // Select menu state
    while(player->base.health > 0 && monster->base.health > 0) {
        switch (current_state) {
        case MENU_COMBAT:
            current_state = combat_menu(player, monster);
            break;
        case MENU_ABILITY:
            if(ability_menu(player, monster)){
                int damage_dealt = use_ability(&monster->base, &player->base, get_random_ability(&monster->base));
                display_enemy_attack_message(player, monster, damage_dealt);
            }
            current_state = MENU_COMBAT;
            break;
        case MENU_ITEM:
            if (item_menu(player, monster)) {  // Only attack if item was used
                int damage_dealt = use_ability(&monster->base, &player->base, get_random_ability(&monster->base));
                display_enemy_attack_message(player, monster, damage_dealt);
            }
            current_state = MENU_COMBAT;
            break;
        default:
            break;
        }
        
    }

    // Return true if player is alive, false if they died
    return (player->base.health > 0);
}

combat_state_t combat_menu(player_t *player, monster_t *monster) {

    int selected_index = 0;
    const char *menu_options[] = {"Use Ability", "Use Item"};
    int menu_count = sizeof(menu_options) / sizeof(menu_options[0]);

    while (true) {

        // Prepare screen
        tb_clear();
        int y = print_combat_view(&player->base, &monster->base, false);

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

bool ability_menu(player_t *player, monster_t *monster) {
    
    int selected_index = 0;
    int ability_count = player->base.ability_count;
    bool ability_used = false;

    while (true) {
        
        // Prepare screen
        tb_clear();
        int y = print_combat_view(&player->base, &monster->base, false);

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
                int damage_dealt;
                damage_dealt = use_ability(&player->base, &monster->base, &player->base.abilities[selected_index]);

                // Change monster sprite color to red
                tb_clear();
                y = print_combat_view(&player->base, &monster->base, true);

                // Display attack message
                char message[100];
                if (damage_dealt <= 0) damage_dealt = 0;
                // TODO: There is no special message for missed or critical hits
                snprintf(message, sizeof(message), "Used %s! Dealt %d damage. Press any key to continue...", player->base.abilities[selected_index].name, damage_dealt);
                tb_print(1, y++, TB_WHITE, TB_DEFAULT, message);
                tb_present();
                usleep(500000);

                // Restore monster sprite color
                y = print_combat_view(&player->base, &monster->base, false);
                tb_present();

                // Wait for any key press to continue
                tb_poll_event(&event);
                ability_used = true;
                break; // Exit the menu after using the ability
            } else if (event.key == TB_KEY_ESC) {
                // Go back to the combat menu
                break;
            }
        }
    }
    return ability_used;
}

bool item_menu(player_t *player, monster_t *monster) {

    int selected_index = 0;
    int usable_item_count = 0;
    usable_item_t *usable_items[MAX_ITEMS];
    bool item_used = false;

    // Collect all usable items
    for (int i = 0; i < player->item_count; i++) {
        if (player->inventory[i]->type == ITEM_TYPE_USABLE) {
            usable_items[usable_item_count++] = (usable_item_t *)player->inventory[i];
        }
    }

    while (true) {

        // Prepare screen
        tb_clear();
        int y = print_combat_view(&player->base, &monster->base, false);

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
                use_item(player, monster, usable_items[selected_index]);
                item_used = true;
                break; // Exit the menu after using the item
            } else if (event.key == TB_KEY_ESC) {
                // Go back to the combat menu
                break;
            }
        }
    }
    return item_used;
}

int use_ability(character_t *attacker, character_t *defender, ability_t *ability) {
    // Roll to hit
    if (roll_hit(ability, defender)) {
        // Roll damage
        int damage = roll_damage(ability);
        return deal_damage(damage, ability->damageType, defender);
    }
    return 0;
}

bool roll_hit(ability_t *ability, character_t *defender) {
    int roll = roll_dice(D20);
    switch (ability->damageType) {
        case PHYSICAL:
            return roll + ability->accuracy > defender->deflection;
            break;
        case MAGICAL:
            return roll + ability->accuracy > defender->fortitude;
            break;
        default:
            break;
    }
    return false;
}

int roll_damage(ability_t *ability) {
    int roll = 0;
    // Roll the dice several times
    for (int i = 0; i < ability->rollCount; i++){
        roll += roll_dice(ability->diceSize);
    }
    return roll;
}

int deal_damage(int damage, damage_type_t damage_type, character_t *character) {
    /* TODO critical hits are ignored */
    if (character->type == MONSTER) {
        monster_t *monster = (monster_t *)character;
        damage += get_weakness_value(monster, damage_type);
    }
    damage -= character->armor;
    if (damage > 0) character->health -= damage;

    return damage;
}

void use_item(player_t *player,monster_t *monster, usable_item_t *item) {
    switch (item->effectType) {
        case HEALING:
            player->base.health += item->value;
            display_item_message(player, monster, item);
            break;
        case ARMOR_INCREASE:
            player->base.armor += item->value;
            display_item_message(player, monster, item);
            break;
        default:
            break;
    }

    for (int i = 0; i < player->item_count; i++) {
        if (player->inventory[i] == (item_t *)item) {
            for (int j = i; j < player->item_count - 1; j++) {
                player->inventory[j] = player->inventory[j + 1];
            }
            player->inventory[player->item_count - 1] = NULL;
            player->item_count--;
            break;
        }
    }
}

int print_combat_view(character_t *player, character_t *monster, bool red_monster_sprite){

    int y = 1;

    // Display player info
    char player_info[100];
    snprintf(player_info, sizeof(player_info), "Player: %s | Health %d", player->name, player->health);
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, player_info);

    // Display monster info
    char monster_info[100];
    snprintf(monster_info, sizeof(monster_info), "Monster: %s | Health %d", monster->name, monster->health);
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, monster_info);

    y += 2;
    for (int i = 0; i < 20; i++) {
        tb_printf(1, y, TB_WHITE, TB_DEFAULT, "");
    }

    // Display monster sprite
    char monster_sprite[100];
    snprintf(monster_sprite, sizeof(monster_sprite), "  (\\_/)\n  (o.o) \n  <( )>  \n");


    if (red_monster_sprite) {
        tb_print(1, y++, TB_RED, TB_DEFAULT, monster_sprite);
    } else {
        tb_print(1, y++, TB_WHITE, TB_DEFAULT, monster_sprite);
    }

    // Add two empty lines after the monster sprite
    y += 2;
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, "");
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, "");

    return y;
}

int roll_dice(dice_size_t dice_size) {
    /* TODO better randomness? (warning message)*/
    return rand() % dice_size + 1;
}

ability_t *get_random_ability(character_t *character){
    int random_index = rand() % character->ability_count;
    return &character->abilities[random_index];
}

void display_enemy_attack_message(player_t *player, monster_t *monster, int damage_dealt) {
    if (damage_dealt <= 0) damage_dealt = 0;
    char message[100];
    snprintf(message, sizeof(message), "Enemy %s attacked! Dealt %d damage. Press any key to continue...", monster->base.name, damage_dealt);
    display_combat_message(player, monster, message);
}

void display_item_message(player_t *player, monster_t *monster, usable_item_t *item) {
    tb_clear();
    char message[100];
    switch (item->effectType) {
        case HEALING:
            snprintf(message, sizeof(message), "Used %s! Healed %d. Press any key to continue...", item->base.name, item->value);
            display_combat_message(player, monster, message);
            break;
        case ARMOR_INCREASE:
            snprintf(message, sizeof(message), "Used %s! Increased armor by %d. Press any key to continue...", item->base.name, item->value);
            display_combat_message(player, monster, message);
            break;
        default:
            break;
    }
}

void display_combat_message(player_t *player, monster_t *monster, const char *message) {
    tb_clear();
    int y = print_combat_view(&player->base, &monster->base, false);
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, message);
    tb_present();

    struct tb_event event;
    tb_poll_event(&event);
}
