#include "npc.h"
#include "pc.h"
#include "combat_mode.h"
#include "../logging/logger.h"
#include "../include/termbox2.h"

#define ABILITY_MENU_STRING "Use Ability"
#define ITEM_MENU_STRING "Use Item"

typedef enum {
    COMBAT_MENU,
    ABILITY_MENU,
    ITEM_MENU,
    EVALUATE_COMBAT, //checks if the combat reached an end
    EXIT //means exit combat & game
} internal_combat_state_t;

// === Internal Functions ===
internal_combat_state_t combat_menu(const player_t* player, const monster_t* monster);
internal_combat_state_t ability_menu(player_t* player, monster_t* monster);
internal_combat_state_t item_menu(player_t* player, monster_t* monster);
int print_combat_view(character_t* player_base, character_t* monster_base, bool red_monster_sprite);
int use_ability(bool player_defend, player_t* player, monster_t* monster, const ability_t* ability);
void use_item(player_t *player, const monster_t *monster, usable_item_t *item);
int roll_dice(dice_size_t dice_size);
void display_item_message(const player_t *player, const monster_t *monster, usable_item_t *item);
void display_combat_message(const player_t *player, const monster_t *monster, const char *message);
void display_enemy_attack_message(const player_t* player, const monster_t* monster, int damage_dealt);
ability_t* get_random_ability(const character_t* character);



combat_result_t start_combat(player_t* player, monster_t* monster) {
    // initial combat state
    internal_combat_state_t combat_state = COMBAT_MENU;
    combat_result_t combat_result = EXIT_GAME;
    bool combat_active = true;

    while(combat_active) {
        int damage_dealt = 0;

        switch (combat_state) {
            case COMBAT_MENU:
                combat_state = combat_menu(player, monster);
                break;
            case ABILITY_MENU:
                combat_state = ability_menu(player, monster);
                damage_dealt = use_ability(true, player, monster, get_random_ability(monster->base));
                display_enemy_attack_message(player, monster, damage_dealt);
                break;
            case ITEM_MENU:
                combat_state = item_menu(player, monster);
                damage_dealt = use_ability(true, player, monster, get_random_ability(monster->base));
                display_enemy_attack_message(player, monster, damage_dealt);
                break;
            case EVALUATE_COMBAT:
                // evaluate the combat result
                if (player->base->health <= 0) {
                    combat_result = PLAYER_LOST;
                    combat_active = false; // exit the combat loop
                } else if (monster->base->health <= 0) {
                    combat_result = PLAYER_WON;
                    combat_active = false; // exit the combat loop
                } else {
                    combat_state = COMBAT_MENU;
                }
                break;
            case EXIT:
                combat_result = EXIT_GAME;
                combat_active = false; // exit the combat loop
                break;
        }
    }

    return combat_result;
}

internal_combat_state_t combat_menu(const player_t* player, const monster_t* monster) {
    int selected_index = 0;
    const char *menu_options[] = {ABILITY_MENU_STRING, ITEM_MENU_STRING};
    const int menu_count = sizeof(menu_options) / sizeof(menu_options[0]);

    internal_combat_state_t new_state = COMBAT_MENU;
    bool submenu_selected = false;

    while (!submenu_selected) {
        // prepare screen
        tb_clear();
        int y = print_combat_view(player->base, monster->base, false);

        // Display menu options
        tb_print(1, y++, TB_WHITE, TB_DEFAULT, "Menu:");

        for(int i = 0; i < menu_count; i++) {
            if (i == selected_index) {
                tb_print(1, y++, TB_BLACK, TB_WHITE, menu_options[i]);
            } else {
                tb_print(1, y++, TB_WHITE, TB_DEFAULT, menu_options[i]);
            }
        }

        y += 2;
        tb_print(1, y, TB_WHITE, TB_DEFAULT, "[ESC] Return to menu");

        // Print to terminal and check for key press
        tb_present();
        struct tb_event event;
        const int ret = tb_peek_event(&event, 10);

        if (ret == TB_OK) {
            if (event.key == TB_KEY_ARROW_UP) {
                // Move up
                selected_index = (selected_index - 1 + menu_count) % menu_count;
            } else if (event.key == TB_KEY_ARROW_DOWN) {
                // Move down
                selected_index = (selected_index + 1) % menu_count;
            } else if (event.key == TB_KEY_ENTER) {
                // Return the selected state
                if (selected_index == 0) {
                    new_state = ABILITY_MENU;
                } else if (selected_index == 1) {
                    new_state = ITEM_MENU;
                }
                submenu_selected = true;
            } else if (event.key == TB_KEY_CTRL_C) {
                // Exit the game
                new_state = EXIT;
                submenu_selected = true;
            }
        }
    }
    return new_state;
}

internal_combat_state_t ability_menu(player_t* player, monster_t* monster) {
    int selected_index = 0;
    const int ability_count = player->base->ability_count;

    internal_combat_state_t new_state = ABILITY_MENU;
    bool ability_used = false;

    while (!ability_used) {
        // Prepare screen
        tb_clear();
        int y = print_combat_view(player->base, monster->base, false);

        // Display menu options
        tb_print(1, y++, TB_WHITE, TB_DEFAULT, "Abilities:");
        for(int i = 0; i < ability_count; i++){
            if (i == selected_index) {
                tb_print(1, y++, TB_WHITE, TB_WHITE, player->base->abilities[i]->name);
            } else {
                tb_print(1, y++, TB_WHITE, TB_DEFAULT, player->base->abilities[i]->name);
            }
        }

        y += 2;
        tb_print(1, y, TB_WHITE, TB_DEFAULT, "[ESC] Return to menu");

        // print to terminal and check for key press
        tb_present();
        struct tb_event event;
        const int ret = tb_peek_event(&event, 10);

        if (ret == TB_OK) {
            if (event.key == TB_KEY_ARROW_UP) {
                // Move up
                selected_index = (selected_index - 1 + ability_count) % ability_count;
            } else if (event.key == TB_KEY_ARROW_DOWN) {
                // Move down
                selected_index = (selected_index + 1) % ability_count;
            } else if (event.key == TB_KEY_ENTER) {
                int damage_dealt = use_ability(false, player, monster, player->base->abilities[selected_index]);

                // Change monster sprite color to red
                tb_clear();
                y = print_combat_view(player->base, monster->base, true);

                // Display attack message
                char message[256];
                if (damage_dealt <= 0) damage_dealt = 0; // What does this line do?
                // TODO: There is no special message for missed or critical hits
                snprintf(message, sizeof(message), "Used %s! Dealt %d damage. Press any key to continue...", player->base->abilities[selected_index]->name, damage_dealt);
                tb_print(1, y++, TB_WHITE, TB_DEFAULT, message);
                tb_present();
                usleep(500000);

                // Restore monster sprite color
                y = print_combat_view(player->base, monster->base, false);
                tb_present();

                usleep(500000);
                new_state = EVALUATE_COMBAT;
                ability_used = true;
            } else if (event.key == TB_KEY_ESC) {
                // go back to the combat menu
                new_state = COMBAT_MENU;
                ability_used = true;
            }
        }

    }
    return new_state;
}

internal_combat_state_t item_menu(player_t* player, monster_t* monster) {

    int selected_index = 0;
    int usable_item_count = 0;
    usable_item_t* usable_items[MAX_ITEMS];

    internal_combat_state_t new_state = ITEM_MENU;
    bool item_used = false;

    // Collect all usable items
    for (int i = 0; i < player->item_count; i++) {
        if (player->usable_items[i] != NULL) {
            //can lead to potential inconsistencie with the inventory and the max capacitiy
            // if the add item function doesn't work as intendend.
            usable_items[usable_item_count++] = player->usable_items[i];
        }
    }

    while (!item_used) {
        // Prepare screen
        tb_clear();
        int y = print_combat_view(player->base, monster->base, false);

        // Display menu options
        tb_print(1, y++, TB_WHITE, TB_DEFAULT, "Usable Items:");
        for (int i = 0; i < usable_item_count; i++) {
            if (i == selected_index) {
                tb_print(1, y++, TB_BLACK, TB_WHITE, usable_items[i]->name);
            } else {
                tb_print(1, y++, TB_WHITE, TB_DEFAULT, usable_items[i]->name);
            }
        }

        y += 2;
        tb_print(1, y, TB_WHITE, TB_DEFAULT, "[ESC] Return to menu");

        // print to terminal and check for key press
        tb_present();
        struct tb_event event;
        const int ret = tb_peek_event(&event, 10);

        if (ret == TB_OK) {
            if (event.key == TB_KEY_ARROW_UP) {
                // Move up
                selected_index = (selected_index - 1 + usable_item_count) % usable_item_count;
            } else if (event.key == TB_KEY_ARROW_DOWN) {
                // Move down
                selected_index = (selected_index + 1) % usable_item_count;
            } else if (event.key == TB_KEY_ENTER) {
                // Use the selected item
                use_item(player, monster, usable_items[selected_index]);
                new_state = ITEM_MENU;
                item_used = true;
            } else if (event.key == TB_KEY_ESC) {
                // Go back to the combat menu
                new_state = COMBAT_MENU;
                item_used = true;
            }
        }

    }
    return new_state;
}

int print_combat_view(character_t* player_base, character_t* monster_base, bool red_monster_sprite) {
    int y = 1;

    // Display player info
    char player_info[100];
    snprintf(player_info, sizeof(player_info), "Player: %s | Health %d", player_base->name, player_base->health);
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, player_info);

    // Display monster info
    char monster_info[100];
    snprintf(monster_info, sizeof(monster_info), "Monster: %s | Health %d", monster_base->name, monster_base->health);
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

int use_ability(const bool player_defend, player_t* player, monster_t* monster, const ability_t* ability) {
    //does use_ability need an return value?
    // Roll to hit
    if (player_defend) {
        if (roll_hit(player->base, ability)) {
            // Roll damage
            const int damage = roll_damage(ability);
            return deal_damage_to_player(damage, ability->damage_type, player);
        }
    } else {
        if (roll_hit(monster->base, ability)) {
            // Roll damage
            const int damage = roll_damage(ability);
            return deal_damage_to_monster(damage, ability->damage_type, monster);
        }
    }
    return 0;
}

void use_item(player_t* player, const monster_t* monster, usable_item_t* item) {
    switch (item->effectType) {
        case HEALING:
            player->base->health += item->value;
            display_item_message(player, monster, item);
            break;
        case ARMOR_INCREASE:
            player->base->armor += item->value;
            display_item_message(player, monster, item);
            break;
        default:
            log_msg(WARNING, "Combat Mode", "Unknown item effect type in use_item");
            break;
    }

    for (int i = 0; i < player->item_count; i++) {
        if (player->usable_items[i] == item) {
            for (int j = i; j < player->item_count - 1; j++) {
                player->usable_items[j] = player->usable_items[j + 1];
            }
            player->usable_items[player->item_count - 1] = NULL;
            player->item_count -= 1;
            break;
        }
    }
}

bool roll_hit(const character_t* defender, const ability_t* ability) {
    const int roll = roll_dice(D20);
    bool hit = false;

    switch (ability->damage_type) {
        case PHYSICAL:
            hit = roll + ability->accuracy > defender->deflection;
            break;
        case MAGICAL:
            hit = roll + ability->accuracy > defender->fortitude;
            break;
        default:
            log_msg(WARNING, "Combat Mode", "Unknown damage type was used, properly DAMAGE_TYPE_COUNT");
            break;
    }
    return hit;
}

int roll_damage(const ability_t* ability) {
    int roll = 0;
    // Roll the dice several times
    for (int i = 0; i < ability->roll_count; i++){
        roll += roll_dice(ability->dice_size);
    }
    return roll;
}

int roll_dice(const dice_size_t dice_size) {
    // TODO better randomness? (warning message)
    return rand() % dice_size + 1;
}

int deal_damage_to_monster(int damage, const damage_type_t damage_type, monster_t* monster) {
    // TODO critical hits are ignored
    damage += get_weakness_value(monster, damage_type);
    damage -= monster->base->armor;
    if (damage > 0) monster->base->health -= damage;
    return damage;
}

int deal_damage_to_player(int damage, const damage_type_t damage_type, player_t* player) {
    // TODO critical hits are ignored
    damage -= player->base->armor;
    if (damage > 0) player->base->health -= damage;
    return damage;
}

void display_item_message(const player_t *player, const monster_t *monster, usable_item_t *item) {
    tb_clear();
    char message[256];
    switch (item->effectType) {
        case HEALING:
            snprintf(message, sizeof(message), "Used %s! Healed %d. Press any key to continue...", item->name, item->value);
            display_combat_message(player, monster, message);
            break;
        case ARMOR_INCREASE:
            snprintf(message, sizeof(message), "Used %s! Increased armor by %d. Press any key to continue...", item->name, item->value);
            display_combat_message(player, monster, message);
            break;
        default:
            break;
    }
}

void display_combat_message(const player_t* player, const monster_t* monster, const char *message) {
    tb_clear();
    int y = print_combat_view(player->base, monster->base, false);
    y += 1;
    tb_print(1, y, TB_WHITE, TB_DEFAULT, message);
    tb_present();

    usleep(10000000); //sleep for 10 seconds
}


void display_enemy_attack_message(const player_t* player, const monster_t* monster, int damage_dealt) {
    if (damage_dealt <= 0) damage_dealt = 0;
    char message[256];
    snprintf(message, sizeof(message), "Enemy %s attacked! Dealt %d damage. Press any key to continue...", monster->base->name, damage_dealt);
    display_combat_message(player, monster, message);
}

ability_t* get_random_ability(const character_t* character) {
    const int random_index = rand() % character->ability_count;
    return character->abilities[random_index];
}
