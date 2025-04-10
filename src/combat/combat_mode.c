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
void use_ability(character_t* attacker, character_t* target, const ability_t* ability);
void use_item(character_t* player, const character_t* monster, item_t* item);
bool use_usable_item(character_t* character, item_t* item);
void display_ability_options(int y, int selected_index, int option_count, const ability_t* abilities[]);
void display_item_options(int y, int selected_index, int option_count, const item_t* items[]);
int display_combat_view(const character_t* player, const character_t* monster, bool red_monster_sprite);
void display_item_message(const character_t* player, const character_t* monster, usable_item_t* item);
void display_combat_message(const character_t* player, const character_t* monster, const char *message, bool red_monster_sprite);
void display_attack_message(const character_t* attacker, const character_t* target, const ability_t* ability, int damage_dealt);
void display_missed_message(const character_t* player, const character_t* monster, const ability_t* ability);
ability_t* get_random_ability(const character_t* character);



combat_result_t start_combat(character_t* player, character_t* monster) {
    // initial combat state
    internal_combat_state_t combat_state = COMBAT_MENU;
    combat_result_t combat_result = EXIT_GAME;
    bool combat_active = true;

    while(combat_active) {
        switch (combat_state) {
            case COMBAT_MENU:
                combat_state = combat_menu(player, monster);
                break;
            case ABILITY_MENU:
                combat_state = ability_menu(player, monster);
                break;
            case ITEM_MENU:
                combat_state = item_menu(player, monster);
                break;
            case EVALUATE_COMBAT:
                // evaluate the combat result
                if (player->current_resources.health <= 0) {
                    combat_result = PLAYER_LOST;
                    combat_active = false; // exit the combat loop
                } else if (monster->current_resources.health <= 0) {
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
        display_ability_options(y, selected_index, player->ability_count, &player->abilities);

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
                use_ability(player, monster, player->abilities[selected_index]);
                use_ability(monster, player, get_random_ability(monster));

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
        display_item_options(y, selected_index, player->item_count, &player->items);

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
                use_ability(monster, player, get_random_ability(monster));
                new_state = EVALUATE_COMBAT;
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
    snprintf(player_info, sizeof(player_info), "Player: %s | Health %d", player->name, player->current_resources.health);
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, player_info);

    // Display monster info
    char monster_info[100];
    snprintf(monster_info, sizeof(monster_info), "Monster: %s | Health %d", monster->name, monster->current_resources.health);
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

void use_ability(character_t* attacker, character_t* target, const ability_t* ability) {
    tb_clear();
    if (roll_hit(attacker, ability)) {
        // TODO: Implement critical hit
        int damage_dealt = deal_damage(target, ability->damage_type,  roll_damage(ability));
        display_attack_message(attacker, target, ability, damage_dealt);
    } else {
        display_missed_message(attacker, target, ability);
    }
    tb_present();
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
            display_combat_message(player, monster, message, false);
            break;
        default:
            break;
    }
}

void display_combat_message(const character_t* attacker, const character_t* target, const char* message, bool red_monster_sprite) {
    tb_clear();
    int y;
    if (attacker->type == PLAYER) {
        y = display_combat_view(attacker, target, red_monster_sprite);
    } else {
        y = display_combat_view(target, attacker, red_monster_sprite);
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

ability_t* get_random_ability(const character_t* character) {
    const int random_index = rand() % character->ability_count;
    return character->abilities[random_index];
}

bool use_usable_item(character_t* character, item_t* item) {
    if (item->type != USABLE) {
        log_msg(ERROR, "Character", "%s cannot use usable_item %s", character->name, item->name);
        return false;
    }
    const usable_item_t* usable_item = (usable_item_t*) (item->extension);

    switch (usable_item->effectType) {
        case HEALING:
            if (usable_item->value > (character->max_resources.health - character->current_resources.health)) {
                character->current_resources.health = character->max_resources.health;
            } else {
                character->current_resources.health += usable_item->value;
            }
            break;
        default:
            log_msg(ERROR, "Character", "Unknown usable_item effect type: %d", usable_item->effectType);
            break;
    }

    remove_item(character, item);
    return true;
}

void display_ability_options(int y, int selected_index, int option_count, const ability_t* abilities[]) {
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, "Abilities:");
        for(int i = 0; i < option_count; i++){
            if (i == selected_index) {
                tb_print(1, y++, TB_WHITE, TB_WHITE, abilities[i]->name);
            } else {
                tb_print(1, y++, TB_WHITE, TB_DEFAULT, abilities[i]->name);
            }
        }
        tb_print(1, y + 2, TB_WHITE, TB_DEFAULT, "[ESC] Return to menu");
}

void display_item_options(int y, int selected_index, int option_count, const item_t* items[]) {
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, "Usable Items:");
        for(int i = 0; i < option_count; i++){
            if (i == selected_index) {
                tb_print(1, y++, TB_WHITE, TB_WHITE, items[i]->name);
            } else {
                tb_print(1, y++, TB_WHITE, TB_DEFAULT, items[i]->name);
            }
        }
        tb_print(1, y + 2, TB_WHITE, TB_DEFAULT, "[ESC] Return to menu");
}