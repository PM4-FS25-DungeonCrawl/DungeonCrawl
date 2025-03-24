#include "combat_mode.h"

void combat(Player *player, Monster *monster) {

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

    // Handle state after combat
    if (player->health <= 0) {
        player_died();
    } else if (monster->health <= 0) {
        player_won();
    }
}

combat_state combat_menu(Player *player, Monster *monster) {
    
    // Clear screen
    tb_clear();
    // Set starting point in y axis for termbox2   
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
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, "Combat Menu:");
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, "1. Use Ability");
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, "2. Use Item");
    tb_present();

    // Get player input
    struct tb_event event;
    combat_state next_state = MENU_COMBAT;

    // Wait for an event
    tb_poll_event(&event);

    if (event.type == TB_EVENT_KEY) {
        if (event.ch == '1') {
            next_state = MENU_ABILITY;
        } else if (event.ch == '2') {
            next_state = MENU_ITEM;
        } else if (event.key == TB_KEY_CTRL_C) {
            tb_shutdown();
            exit(0);
        }
    }
    return next_state;
}

void ability_menu(Player *player, Monster *monster) {
    /* TODO */
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

void player_won() {
    /* TODO */
}

void player_died() {
    /* TODO */
}