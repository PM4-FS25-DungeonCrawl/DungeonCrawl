#include "combat_mode.h"

#include "../character/character.h"
#include "../character/level.h"
#include "../include/termbox2.h"
#include "../item/potion.h"
#include "./draw/draw_combat_mode.h"
#include "ability.h"

#define ABILITY_MENU_STRING "Use Ability"
#define ITEM_MENU_STRING "Use Potion"
#define MAX_OPTION_LENGTH 256

typedef enum {
    COMBAT_MENU,
    ABILITY_MENU,
    ITEM_MENU,
    EVALUATE_COMBAT,//checks if the combat reached an end
    EXIT            //means exit combat & game
} internal_combat_state_t;

// === Internal Functions ===
internal_combat_state_t combat_menu(const character_t* player, const character_t* monster);
internal_combat_state_t ability_menu(character_t* player, character_t* monster);
internal_combat_state_t potion_menu(character_t* player, character_t* monster);
void use_ability(character_t* attacker, character_t* target, const ability_t* ability);
void use_item(character_t* player, const character_t* monster, potion_t* potion);
bool invoke_potion_effect(character_t* character, potion_t* potion);
bool consume_ability_resource(character_t* attacker, const ability_t* ability);
ability_t* get_random_ability(const character_t* character);
char** generate_ability_menu_options(ability_t* abilities[], int count);
char** generate_potion_menu_options(potion_t* potions[], int count);

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
                combat_state = potion_menu(player, monster);
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
            case EXIT:
                combat_result = EXIT_GAME;
                combat_active = false;// exit the combat loop
                break;
        }
    }

    return combat_result;
}

internal_combat_state_t combat_menu(const character_t* player, const character_t* monster) {
    // draw combat view
    vector2d_t anchor = draw_combat_view(player, monster, "enemy sprite" , false);
    int selected_index = 0;
 
    const char* menu_options[] = {ABILITY_MENU_STRING, ITEM_MENU_STRING};
    const int menu_option_count = sizeof(menu_options) / sizeof(menu_options[0]);

    internal_combat_state_t new_state = COMBAT_MENU;
    bool submenu_selected = false;

    while (!submenu_selected) {
        // draw menu options
        draw_combat_menu(anchor, "Combat Menu:", menu_options, menu_option_count, selected_index);        

        // check for input
        struct tb_event event;
        const int ret = tb_peek_event(&event, 10);

        if (ret == TB_OK) {
            if (event.key == TB_KEY_ARROW_UP) {
                // Move up
                selected_index = (selected_index - 1 + menu_option_count) % menu_option_count;
            } else if (event.key == TB_KEY_ARROW_DOWN) {
                // Move down
                selected_index = (selected_index + 1) % menu_option_count;
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
    // draw combat view
    vector2d_t anchor = draw_combat_view(player, monster, "enemy sprite" , false);
    int selected_index = 0;

    const int ability_count = player->ability_count;
    const char** menu_options = generate_ability_menu_options(&player->abilities, ability_count);
    
    internal_combat_state_t new_state = ABILITY_MENU;
    bool ability_used = false;

    while (!ability_used) {
        // draw menu options
        draw_combat_menu(anchor, "Ability Menu:", menu_options, ability_count, selected_index); 

        // check for input
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

internal_combat_state_t potion_menu(character_t* player, character_t* monster) {
    // draw combat view
    vector2d_t anchor = draw_combat_view(player, monster, "Enemey Sprite", false);
    int selected_index = 0;

    if (player->potion_inventory_count == 0) {
        char message[256];
        snprintf(message, sizeof(message), "You search your bag... but there are no potions left.");
        draw_combat_log(anchor, message);
        return COMBAT_MENU;
    }
    
    int potion_count = player->potion_inventory_count;
    const char** menu_options = generate_potion_menu_options(&player->potion_inventory, potion_count);

    internal_combat_state_t new_state = ITEM_MENU;
    bool item_used = false;

    while (!item_used) {
        // draw menu options
        draw_combat_menu(anchor, "Potion menu:", menu_options, potion_count, selected_index);

        // check for input
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
                // Use the selected potion
                use_item(player, monster, player->potion_inventory[selected_index]);
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
    character_t* player;
    character_t* monster;
    bool sprite;
    char message[256];
    if (attacker->type == PLAYER) {
        player = attacker;
        monster = target;
        sprite = true;
    } else {
        player = target;
        monster = attacker;
        sprite = false;
    }
    
    vector2d_t anchor = draw_combat_view(player, monster, "Enemy Sprite", false);
    if (consume_ability_resource(attacker, ability)) {
        if (roll_hit(attacker->current_stats.dexterity, target->current_stats.dexterity)) {
            int damage_dealt = deal_damage(target, ability->damage_type, roll_damage(ability));

            draw_combat_view(player, monster, "Enemy Sprite", sprite);
            
            memset(message, 0, sizeof(message));
            snprintf(message, sizeof(message), "%s uses %s and deals %d %s damage to %s!",
                attacker->name,
                ability->name,
                damage_dealt,
                damage_type_to_string(ability->damage_type),
                target->name);
            draw_combat_log(anchor, message);
        } else {
            draw_combat_view(player, monster, "Enemy Sprite", false);

            memset(message, 0, sizeof(message));
            snprintf(message, sizeof(message), "%s uses %s, but it missed!",
                attacker->name,
                ability->name);
            draw_combat_log(anchor, message);
        }
    } else {
        memset(message, 0, sizeof(message));
        snprintf(message, sizeof(message), "%s tries to cast %s, but doesn't have enough resources!",
            attacker->name,
            ability->name);
        draw_combat_log(anchor, message);
    }
    tb_present();
}

void use_item(character_t* player, const character_t* monster, potion_t* potion) {
    vector2d_t anchor = draw_combat_view(player, monster, "Enemy Sprite", false);
    bool consumed = invoke_potion_effect(player, potion);
    if (consumed) {

        char message[256];
        snprintf(message, sizeof(message), "%s uses a %s potion, restoring %d %s!",
            player->name,
            potion->name,
            potion->value,
            potion_type_to_string(potion->effectType));
        draw_combat_log(anchor, message);
    }
}

ability_t* get_random_ability(const character_t* character) {
    const int random_index = rand() % character->ability_count;
    return character->abilities[random_index];
}

bool invoke_potion_effect(character_t* character, potion_t* potion) {
    switch (potion->effectType) {
        case HEALING:
            if (potion->value > (character->max_resources.health - character->current_resources.health)) {
                character->current_resources.health = character->max_resources.health;
            } else {
                character->current_resources.health += potion->value;
            }
            break;
        default:
            log_msg(ERROR, "Character", "Unknown usable_item effect type: %d", potion->effectType);
            break;
    }
    remove_potion(character, potion);
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

// Helper function to create ability options array
char** generate_ability_menu_options(ability_t* abilities[], int count) {
    char** menu_options = malloc(count * sizeof(char*));

    for (int i = 0; i < count; ++i) {
        menu_options[i] = malloc(MAX_OPTION_LENGTH);
        snprintf(menu_options[i], MAX_OPTION_LENGTH,
                 "%-16s - Rolls: %-2d, Accuracy: %-3d%%, Cost: %-3d, Dice: %-4s, Type: %-10s",
                 abilities[i]->name,
                 abilities[i]->roll_amount,
                 abilities[i]->accuracy,
                 abilities[i]->resource_cost,
                 dice_size_to_string(abilities[i]->dice_size),
                 damage_type_to_string(abilities[i]->damage_type));
    }
    return menu_options;
}

// Helper function to create potion options array
char** generate_potion_menu_options(potion_t* potions[], int count) {
    char** menu_options = malloc(count * sizeof(char*));

    for (int i = 0; i < count; ++i) {
        menu_options[i] = malloc(MAX_OPTION_LENGTH);
        snprintf(menu_options[i], MAX_OPTION_LENGTH,
                 "%-15s  Type: %-10s  Value: %-3d",
                 potions[i]->name,
                 potion_type_to_string(potions[i]->effectType),
                 potions[i]->value);
    }
    return menu_options;
}