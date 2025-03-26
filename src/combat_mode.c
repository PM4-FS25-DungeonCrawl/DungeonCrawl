#include "combat_mode.h"

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
    
    // Prepare screen
    tb_clear(); 
    int y = 1;

    // Display player info
    char player_info[100];
    snprintf(player_info, sizeof(player_info), "Player: %s | Health %d", player->name, player->health);
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, player_info);

    // Display monster info
    char monster_info[100];
    snprintf(monster_info, sizeof(monster_info), "Monster: %s | Health %d", monster->name, monster->health);
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, monster_info);

    // Display menu options
    y += 5;
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, "Combat Menu");
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, "[1] Use Ability");
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, "[2] Use Item");
    tb_present();

    // Get player input
    struct tb_event event;
    combat_state next_state = MENU_COMBAT;

    // Wait for an event
    tb_poll_event(&event);

    if (event.ch == '1') {
        next_state = MENU_ABILITY;
    } else if (event.ch == '2') {
        next_state = MENU_ITEM;
    } else if (event.key == TB_KEY_CTRL_C) {
        tb_shutdown();
        exit(0);
    }
    return next_state;
}

void ability_menu(Player *player, Monster *monster) {
    
    int selected_index = 0;
    int ability_count = player->abilityCount;

    while (true) {
        
        // Prepare screen
        tb_clear();
        int y = 1;

        // Display menu options
        tb_print(1, y++, TB_WHITE, TB_DEFAULT, "Ability Menu [ESC to go back]:");
        for(int i = 0; i < ability_count; i++){
            if (i == selected_index) {
                tb_print(1, y++, TB_WHITE, TB_WHITE, player->abilities[i].name);
            } else {
                tb_print(1, y++, TB_WHITE, TB_DEFAULT, player->abilities[i].name);
            }
        }

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
    /* TODO */
}

bool roll_hit(Player *player, Ability *ability, int dice_size) {
    /* TODO */
    return true;
}

int roll_damage(Player *player, Ability *ability, int dice_size) {
    /* TODO */
    return ability->damageValue;
}

void deal_damage(int damage, Monster *monster) {
    /* TODO */
}

void take_damage(Monster *monster, Player *player) {
    /* TODO */
}

void use_item() {
    /* TODO */
}