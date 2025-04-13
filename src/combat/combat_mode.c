#include "combat_mode.h"

#include "../character/character.h"
#include "../character/level.h"
#include "../include/termbox2.h"
#include "../item/potion.h"
#include "./display/combat_display.h"
#include "ability.h"

combat_result_t start_combat(character_t* player, character_t* monster) {
    // initial combat state
    internal_combat_state_t combat_state = COMBAT_MENU;
    combat_result_t combat_result = EXIT_GAME;
    bool combat_active = true;

    while (combat_active) {
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
                    combat_active = false;// exit the combat loop
                } else if (monster->current_resources.health <= 0) {
                    combat_result = PLAYER_WON;
                    combat_active = false;// exit the combat loop
                    player->xp += monster->xp_reward;
                    if (player->xp >= calculate_xp_for_next_level(player->level)) {
                        level_up(player);
                    }
                } else {
                    combat_state = COMBAT_MENU;
                }
                break;
            case COMBAT_EXIT:
                combat_result = EXIT_GAME;
                combat_active = false;// exit the combat loop
                break;
        }
    }

    return combat_result;
}

internal_combat_state_t combat_menu(const character_t* player, const character_t* monster) {
    int selected_index = 0;
    const char* menu_options[] = {ABILITY_MENU_STRING, ITEM_MENU_STRING};
    const int menu_count = sizeof(menu_options) / sizeof(menu_options[0]);

    internal_combat_state_t new_state = COMBAT_MENU;
    bool submenu_selected = false;

    while (!submenu_selected) {
        // prepare screen
        tb_clear();
        int y = display_combat_view(player, monster, false);

        // Display menu options
        tb_print(1, y++, TB_WHITE, TB_DEFAULT, "Menu:");

        for (int i = 0; i < menu_count; i++) {
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
                new_state = COMBAT_EXIT;
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
        display_ability_options(y, selected_index, player->abilities);

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
    if (player->potion_inventory_count == 0) {
        return COMBAT_MENU;
    }
    int selected_index = 0;
    int potion_count = 0;
    potion_t* potions[USABLE_ITEM_LIMIT];

    internal_combat_state_t new_state = ITEM_MENU;
    bool item_used = false;

    // Collect all usable items
    for (int i = 0; i < player->potion_inventory_count; i++) {
        if (player->potion_inventory[i] != NULL) {
            potions[potion_count] = player->potion_inventory[i];
            potion_count++;
        }
    }

    while (!item_used) {
        // Prepare screen
        tb_clear();
        int y = display_combat_view(player, monster, false);

        // Display menu options
        display_potion_options(y, selected_index, &player->potion_inventory);

        // print to terminal and check for key press
        tb_present();
        struct tb_event event;
        const int ret = tb_peek_event(&event, 10);

        if (ret == TB_OK) {
            if (event.key == TB_KEY_ARROW_UP) {
                // Move up
                selected_index = (selected_index - 1 + potion_count) % potion_count;
            } else if (event.key == TB_KEY_ARROW_DOWN) {
                // Move down
                selected_index = (selected_index + 1) % potion_count;
            } else if (event.key == TB_KEY_ENTER) {
                // Use the selected item
                use_item(player, monster, potions[selected_index]);
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

void use_ability(character_t* attacker, character_t* target, const ability_t* ability) {
    tb_clear();
    if (consume_ability_resource(attacker, ability)) {
        if (roll_hit(attacker->current_stats.dexterity, target->current_stats.dexterity)) {
            int damage_dealt = deal_damage(target, ability->damage_type, roll_damage(ability));
            display_attack_message(attacker, target, ability, damage_dealt);
        } else {
            display_missed_message(attacker, target, ability);
        }
    } else {
        display_oom_message(attacker, target, ability);
    }

    tb_present();
}

void use_item(character_t* player, const character_t* monster, potion_t* item) {
    bool consumed = use_usable_item(player, item);
    if (consumed) {
        display_potion_message(player, monster, item);
    }
}

ability_t* get_random_ability(const character_t* character) {
    const int random_index = rand() % character->ability_count;
    return character->abilities[random_index];
}

bool use_usable_item(character_t* character, potion_t* item) {
    switch (item->effectType) {
        case HEALING:
            if (item->value > (character->max_resources.health - character->current_resources.health)) {
                character->current_resources.health = character->max_resources.health;
            } else {
                character->current_resources.health += item->value;
            }
            break;
        default:
            log_msg(ERROR, "Character", "Unknown usable_item effect type: %d", item->effectType);
            break;
    }
    remove_potion(character, item);
    return true;
}

bool consume_ability_resource(character_t* attacker, const ability_t* ability) {
    int* resource = NULL;

    switch (ability->damage_type) {
        case PHYSICAL:
            resource = &attacker->current_resources.stamina;
            break;
        case MAGICAL:
            resource = &attacker->current_resources.mana;
            break;
    }

    if (resource != NULL && *resource >= ability->resource_cost) {
        *resource -= ability->resource_cost;
        return true;
    }
    return false;
}
