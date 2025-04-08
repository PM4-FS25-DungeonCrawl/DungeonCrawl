#include "combat_mode.h"
#include "ability.h"
#include "../character/character.h"
#include "../item/base_item.h"
#include "../item/usable_item.h"
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
internal_combat_state_t combat_menu(const character_t* player, const character_t* monster);
internal_combat_state_t ability_menu(character_t* player, character_t* monster);
internal_combat_state_t item_menu(character_t* player, character_t* monster);
int use_ability(bool player_defend, character_t* player, character_t* monster, const ability_t* ability);
void use_item(character_t* player, const character_t* monster, item_t* item);
int display_combat_view(const character_t* player, const character_t* monster, bool red_monster_sprite);
void display_item_message(const character_t* player, const character_t* monster, usable_item_t* item);
void display_combat_message(const character_t* player, const character_t* monster, const char *message);
void display_enemy_attack_message(const character_t* player, const character_t* monster, int damage_dealt);
ability_t* get_random_ability(const character_t* character);



combat_result_t start_combat(character_t* player, character_t* monster) {
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
                damage_dealt = use_ability(true, player, monster, get_random_ability(monster));
                display_enemy_attack_message(player, monster, damage_dealt);
                break;
            case ITEM_MENU:
                combat_state = item_menu(player, monster);
                damage_dealt = use_ability(true, player, monster, get_random_ability(monster));
                display_enemy_attack_message(player, monster, damage_dealt);
                break;
            case EVALUATE_COMBAT:
                // evaluate the combat result
                if (player->current_stats.health <= 0) {
                    combat_result = PLAYER_LOST;
                    combat_active = false; // exit the combat loop
                } else if (monster->current_stats.health <= 0) {
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

internal_combat_state_t combat_menu(const character_t* player, const character_t* monster) {
    int selected_index = 0;
    const char *menu_options[] = {ABILITY_MENU_STRING, ITEM_MENU_STRING};
    const int menu_count = sizeof(menu_options) / sizeof(menu_options[0]);

    internal_combat_state_t new_state = COMBAT_MENU;
    bool submenu_selected = false;

    while (!submenu_selected) {
        // prepare screen
        tb_clear();
        int y = display_combat_view(player, monster, false);

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

internal_combat_state_t ability_menu(character_t* player, character_t* monster) {
    int selected_index = 0;
    const int ability_count = player->ability_count;

    internal_combat_state_t new_state = ABILITY_MENU;
    bool ability_used = false;

    while (!ability_used) {
        // Prepare screen
        tb_clear();
        int y = display_combat_view(player, monster, false);

        // Display menu options
        tb_print(1, y++, TB_WHITE, TB_DEFAULT, "Abilities:");
        for(int i = 0; i < ability_count; i++){
            if (i == selected_index) {
                tb_print(1, y++, TB_WHITE, TB_WHITE, player->abilities[i]->name);
            } else {
                tb_print(1, y++, TB_WHITE, TB_DEFAULT, player->abilities[i]->name);
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
                int damage_dealt = use_ability(false, player, monster, player->abilities[selected_index]);

                // Change monster sprite color to red
                tb_clear();
                y = display_combat_view(player, monster, true);

                // Display attack message
                char message[256];
                if (damage_dealt <= 0) damage_dealt = 0; // What does this line do?
                // TODO: There is no special message for missed or critical hits
                snprintf(message, sizeof(message), "Used %s! Dealt %d damage. Press any key to continue...", player->abilities[selected_index]->name, damage_dealt);
                tb_print(1, y, TB_WHITE, TB_DEFAULT, message);
                tb_present();
                usleep(500000);

                // Restore monster sprite color
                (void) display_combat_view(player, monster, false);
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

internal_combat_state_t item_menu(character_t* player, character_t* monster) {
    int selected_index = 0;
    int usable_item_count = 0;
    usable_item_t* usable_items[ITEM_LIMIT];

    internal_combat_state_t new_state = ITEM_MENU;
    bool item_used = false;

    // Collect all usable items
    for (int i = 0; i < player->item_count; i++) {
        if (player->items[i] != NULL && player->items[i]->type == USABLE) {
            usable_items[usable_item_count] = player->items[i]->extension;
            usable_item_count++;
        }
    }

    while (!item_used) {
        // Prepare screen
        tb_clear();
        int y = display_combat_view(player, monster, false);

        // Display menu options
        tb_print(1, y++, TB_WHITE, TB_DEFAULT, "Usable Items:");
        for (int i = 0; i < usable_item_count; i++) {
            if (i == selected_index) {
                tb_print(1, y++, TB_BLACK, TB_WHITE, usable_items[i]->base->name);
            } else {
                tb_print(1, y++, TB_WHITE, TB_DEFAULT, usable_items[i]->base->name);
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
                use_item(player, monster, usable_items[selected_index]->base);
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

int display_combat_view(const character_t* player, const character_t* monster, bool red_monster_sprite) {
    int y = 1;

    // Display player info
    char player_info[100];
    snprintf(player_info, sizeof(player_info), "Player: %s | Health %d", player->name, player->current_stats.health);
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, player_info);

    // Display monster info
    char monster_info[100];
    snprintf(monster_info, sizeof(monster_info), "Monster: %s | Health %d", monster->name, monster->current_stats.health);
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

//TODO: debug why does this function always return 0, stated by CLion
int use_ability(const bool player_defend, character_t* player, character_t* monster, const ability_t* ability) {
    // does use_ability need a return value?
    int damage_dealt = 0;

    if (player_defend) {
        if (roll_hit(player, ability)) {
            // Roll damage
            int damage = roll_damage(ability);
            damage_dealt = deal_damage(player, ability->damage_type,  damage);
        }
    } else {
        if (roll_hit(monster, ability)) {
            // Roll damage
            int damage = roll_damage(ability);
            damage_dealt = deal_damage(monster, ability->damage_type, damage);
        }
    }
    return damage_dealt;
}

void use_item(character_t* player, const character_t* monster, item_t* item) {
    bool consumed = use_usable_item(player, item);
    if (consumed) {
        display_item_message(player, monster, (usable_item_t*) item->extension);
    }
}

void display_item_message(const character_t* player, const character_t* monster, usable_item_t* item) {
    tb_clear();
    char message[256];
    switch (item->effectType) {
        case HEALING:
            snprintf(message, sizeof(message), "Used %s! Healed %d. Press any key to continue...", item->base->name, item->value);
            display_combat_message(player, monster, message);
            break;
        case ARMOR_INCREASE:
            snprintf(message, sizeof(message), "Used %s! Increased armor by %d. Press any key to continue...", item->base->name, item->value);
            display_combat_message(player, monster, message);
            break;
        default:
            break;
    }
}

void display_combat_message(const character_t* player, const character_t* monster, const char* message) {
    tb_clear();
    int y = display_combat_view(player, monster, false);
    y += 1;
    tb_print(1, y, TB_WHITE, TB_DEFAULT, message);
    tb_present();

    usleep(10000000); //sleep for 10 seconds
}

void display_enemy_attack_message(const character_t* player, const character_t* monster, int damage_dealt) {
    if (damage_dealt <= 0) damage_dealt = 0;
    char message[256];
    snprintf(message, sizeof(message), "Enemy %s attacked! Dealt %d damage. Press any key to continue...", monster->name, damage_dealt);
    display_combat_message(player, monster, message);
}

ability_t* get_random_ability(const character_t* character) {
    const int random_index = rand() % character->ability_count;
    return character->abilities[random_index];
}
