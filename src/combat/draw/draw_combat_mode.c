#include "draw_combat_mode.h"

int display_combat_view(const character_t* attacker, const character_t* target, const bool red_target_sprite) {
    int y = 1;

    // Display player info
    char player_info[256];
    snprintf(player_info, sizeof(player_info), "Player: %s | Health %d | Mana %d | Stamina %d", attacker->name, attacker->current_resources.health, attacker->current_resources.mana, attacker->current_resources.stamina);
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, player_info);

    // Display monster info
    char monster_info[256];
    snprintf(monster_info, sizeof(monster_info), "Monster: %s | Health %d", target->name, target->current_resources.health);
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, monster_info);

    y += 2;
    for (int i = 0; i < 20; i++) {
        tb_printf(1, y, TB_WHITE, TB_DEFAULT, "");
    }

    // Display monster sprite
    char monster_sprite[256];
    snprintf(monster_sprite, sizeof(monster_sprite), "  (\\_/)\n  (o.o) \n  <( )>  \n");// TODO: make a list of sprites and connect to monster

    if (red_target_sprite) {
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

void display_potion_message(const character_t* attacker, const character_t* target, potion_t* potion) {
    tb_clear();
    char message[256];
    switch (potion->effectType) {
        case HEALING:
            snprintf(message, sizeof(message), "Used %s! Healed %d. Press any key to continue...", potion->name, potion->value);
            display_combat_message(attacker, target, message, false);
            break;
        default:
            break;
    }
}

void display_combat_message(const character_t* attacker, const character_t* target, const char* message, bool red_target_sprite) {
    tb_clear();
    int y;
    if (attacker->type == PLAYER) {
        y = display_combat_view(attacker, target, red_target_sprite);
    } else {
        y = display_combat_view(target, attacker, red_target_sprite);
    }
    y += 1;
    tb_print(1, y, TB_WHITE, TB_DEFAULT, message);
    tb_present();

    struct tb_event event;
    tb_poll_event(&event);
}

void display_attack_message(const character_t* attacker, const character_t* target, const ability_t* ability, int damage_dealt) {
    if (damage_dealt <= 0) damage_dealt = 0;
    char message[256];
    snprintf(message, sizeof(message), "%s attacked using %s! Dealt %d damage. Press any key to continue...", attacker->name, ability->name, damage_dealt);
    if (attacker->type == PLAYER) {
        display_combat_message(attacker, target, message, true);
    } else {
        display_combat_message(attacker, target, message, false);
    }
}

void display_missed_message(const character_t* attacker, const character_t* target, const ability_t* ability) {
    char message[256];
    snprintf(message, sizeof(message), "%s attacked using %s! The attack missed :( Press any key to continue...", attacker->name, ability->name);
    display_combat_message(attacker, target, message, false);
}

void display_oom_message(const character_t* attacker, const character_t* target, const ability_t* ability) {
    char message[256];
    switch (ability->damage_type) {
        case PHYSICAL:
            snprintf(message, sizeof(message), "%s has not enough stamina to use %s! Press any key to continue...", attacker->name, ability->name);
            display_combat_message(attacker, target, message, false);
            break;
        case MAGICAL:
            snprintf(message, sizeof(message), "%s has not enough mana to use %s! Press any key to continue...", attacker->name, ability->name);
            display_combat_message(attacker, target, message, false);
            break;
        default:
            break;
    }
}

void display_ability_options(int y, const int selected_index, const ability_t* abilities[]) {
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, "Abilities:");
    for (int i = 0; i < ABILITY_LIMIT; i++) {
        if (abilities[i] != NULL) {
            if (i == selected_index) {
                tb_print(1, y++, TB_WHITE, TB_WHITE, abilities[i]->name);
            } else {
                tb_print(1, y++, TB_WHITE, TB_DEFAULT, abilities[i]->name);
            }
        }
    }
    tb_print(1, y + 2, TB_WHITE, TB_DEFAULT, "[ESC] Return to menu");
}

void display_potion_options(int y, const int selected_index, const potion_t* potions[]) {
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, "Usable Potions:");
    for (int i = 0; i < USABLE_ITEM_LIMIT; i++) {
        if (potions[i] != NULL) {
            if (i == selected_index) {
                tb_print(1, y++, TB_WHITE, TB_WHITE, potions[i]->name);
            } else {
                tb_print(1, y++, TB_WHITE, TB_DEFAULT, potions[i]->name);
            }
        }
    }
    tb_print(1, y + 2, TB_WHITE, TB_DEFAULT, "[ESC] Return to menu");
}