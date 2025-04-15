#include "combat_mode.h"

#include "../asciiart/ascii.h"
#include "../character/character.h"
#include "../character/level.h"
#include "../include/termbox2.h"
#include "./draw/draw_combat_mode.h"
#include "ability.h"
#include "../local/local.h"

#define MAX_COMBAT_MENU_OPTIONS 2

//key define for localization
#define MAIN_MENU_TITLE_KEY "COMBAT.MAIN.MENU.HEAD"
#define MAIN_MENU_OPTION1_KEY "COMBAT.MAIN.MENU.OPTION1"
#define MAIN_MENU_OPTION2_KEY "COMBAT.MAIN.MENU.OPTION2"
#define ABILITY_MENU_TITLE_KEY "COMBAT.ABILITY.MENU.HEAD"
#define ITEM_MENU_TITLE_KEY "COMBAT.ITEM.MENU.HEAD"

// === Internal Functions ===
//TODO: Should these 2 function not be in to character.c?
void invoke_potion_effect(character_t* character, potion_t* potion);

void collect_ability_menu_options(ability_t* abilities[], int count);
void collect_potion_menu_options(potion_t* potions[], int count);

void update_local(void);

// === Intern Global Variables ===
vector2d_t combat_view_anchor = {1, 1};
char** combat_menu_options;
char** ability_menu_options;
char** potion_menu_options;



/**
 * @brief Initialize the combat mode
 * @note This function must be called before using any other functions in this module.
 */
void init_combat_mode(void) {
    combat_menu_options = malloc(sizeof(char*) * MAX_COMBAT_MENU_OPTIONS);
    if (combat_menu_options == NULL) {
        log_msg(ERROR, "Combat Mode", "Failed to allocate memory for combat menu options");
        return;
    }

    for (int i = 0; i < MAX_COMBAT_MENU_OPTIONS; i++) {
        combat_menu_options[i] = (char*) malloc(sizeof(char) * MAX_STRING_LENGTH);
        if (combat_menu_options[i] == NULL) {
            log_msg(ERROR, "Combat Mode", "Failed to allocate memory for combat menu option %d", i);
            shutdown_combat_mode();
            return;
        }
    }

    ability_menu_options = (char**) malloc(sizeof(char*) * MAX_ABILITY_LIMIT);
    if (ability_menu_options == NULL) {
        log_msg(ERROR, "Combat Mode", "Failed to allocate memory for ability menu options");
        shutdown_combat_mode();
        return;
    }

    for (int i = 0; i < MAX_ABILITY_LIMIT; i++) {
        ability_menu_options[i] = (char*) malloc(sizeof(char) * MAX_STRING_LENGTH);
        if (ability_menu_options[i] == NULL) {
            log_msg(ERROR, "Combat Mode", "Failed to allocate memory for ability menu option %d", i);
            shutdown_combat_mode();
            return;
        }
    }

    potion_menu_options = (char**) malloc(sizeof(char*) * MAX_POTION_LIMIT);
    if (potion_menu_options == NULL) {
        log_msg(ERROR, "Combat Mode", "Failed to allocate memory for potion menu options");
        shutdown_combat_mode();
        return;
    }

    for (int i = 0; i < MAX_POTION_LIMIT; i++) {
        potion_menu_options[i] = (char*) malloc(sizeof(char) * MAX_STRING_LENGTH);
        if (potion_menu_options[i] == NULL) {
            log_msg(ERROR, "Combat Mode", "Failed to allocate memory for potion menu option %d", i);
            shutdown_combat_mode();
            return;
        }
    }

    update_local();
    //add update local function to the observer list
    add_observer(update_local);
}


combat_result_t start_combat(character_t* player, character_t* monster) {
    // initial combat state
    internal_combat_state_t combat_state = COMBAT_MENU;
    combat_result_t combat_result = EXIT_GAME;
    bool combat_active = true;

    //collect menu options
    collect_ability_menu_options(player->abilities, player->ability_count);
    collect_potion_menu_options(player->potion_inventory, player->potion_count);

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
            case COMBAT_EXIT:
                combat_result = EXIT_GAME;
                combat_active = false;// exit the combat loop
                break;
        }
    }

    return combat_result;
}

internal_combat_state_t combat_menu(const character_t* player, const character_t* monster) {
    // draw combat view
    const vector2d_t anchor = draw_combat_view(combat_view_anchor, player, monster, ascii_goblin, GOBLIN_HEIGHT, false);
    int selected_index = 0;

    internal_combat_state_t new_state = COMBAT_MENU;
    bool submenu_selected = false;

    while (!submenu_selected) {
        // draw menu options
        draw_combat_menu(anchor, "Combat Menu:", combat_menu_options, MAX_COMBAT_MENU_OPTIONS, selected_index);

        // check for input
        struct tb_event event;
        const int ret = tb_peek_event(&event, 10);

        if (ret == TB_OK) {
            if (event.key == TB_KEY_ARROW_UP) {
                // Move up
                selected_index = (selected_index - 1 + MAX_COMBAT_MENU_OPTIONS) % MAX_COMBAT_MENU_OPTIONS;
            } else if (event.key == TB_KEY_ARROW_DOWN) {
                // Move down
                selected_index = (selected_index + 1) % MAX_COMBAT_MENU_OPTIONS;
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
    // draw combat view
    const vector2d_t anchor = draw_combat_view(combat_view_anchor, player, monster, ascii_goblin, GOBLIN_HEIGHT, false);
    int selected_index = 0;

    internal_combat_state_t new_state = ABILITY_MENU;
    bool ability_used_or_esc = false;

    while (!ability_used_or_esc) {
        // draw menu options
        draw_combat_menu(anchor, "Ability Menu:", (const char**) ability_menu_options, player->ability_count, selected_index);

        // check for input
        struct tb_event event;
        const int ret = tb_peek_event(&event, 10);

        if (ret == TB_OK) {
            if (event.key == TB_KEY_ARROW_UP) {
                // Move up
                selected_index = (selected_index - 1 + player->ability_count) % player->ability_count;
            } else if (event.key == TB_KEY_ARROW_DOWN) {
                // Move down
                selected_index = (selected_index + 1) % player->ability_count;
            } else if (event.key == TB_KEY_ENTER) {
                use_ability(player, monster, player->abilities[selected_index]);
                use_ability(monster, player, get_random_ability(monster));

                new_state = EVALUATE_COMBAT;
                ability_used_or_esc = true;
            } else if (event.key == TB_KEY_ESC) {
                // go back to the combat menu
                new_state = COMBAT_MENU;
                ability_used_or_esc = true;
            }
        }
    }
    return new_state;
}

internal_combat_state_t potion_menu(character_t* player, character_t* monster) {
    // draw combat view
    const vector2d_t anchor = draw_combat_view(combat_view_anchor, player, monster, ascii_goblin, GOBLIN_HEIGHT, false);
    int selected_index = 0;

    if (player->potion_count == 0) {
        char message[MAX_STRING_LENGTH];
        snprintf(message, sizeof(message), "You search your bag... but there are no potions left.");
        draw_combat_log(anchor, message);
        return COMBAT_MENU;
    }

    internal_combat_state_t new_state = ITEM_MENU;
    bool item_used_or_esc = false;

    while (!item_used_or_esc) {
        // draw menu options
        draw_combat_menu(anchor, "Potion menu:", (const char**) potion_menu_options, player->potion_count, selected_index);

        // check for input
        struct tb_event event;
        const int ret = tb_peek_event(&event, 10);

        if (ret == TB_OK) {
            if (event.key == TB_KEY_ARROW_UP) {
                // Move up
                selected_index = (selected_index - 1 + player->potion_count) % player->potion_count;
            } else if (event.key == TB_KEY_ARROW_DOWN) {
                // Move down
                selected_index = (selected_index + 1) % player->potion_count;
            } else if (event.key == TB_KEY_ENTER) {
                // Use the selected potion
                use_potion(player, monster, player->potion_inventory[selected_index]);
                use_ability(monster, player, get_random_ability(monster));
                new_state = EVALUATE_COMBAT;

                collect_potion_menu_options(player->potion_inventory, player->potion_count);
                item_used_or_esc = true;
            } else if (event.key == TB_KEY_ESC) {
                // Go back to the combat menu
                new_state = COMBAT_MENU;
                item_used_or_esc = true;
            }
        }
    }


    return new_state;
}

void use_ability(character_t* attacker, character_t* target, const ability_t* ability) {
    character_t* player;
    character_t* monster;
    bool sprite;
    char message[MAX_STRING_LENGTH];
    if (attacker->type == PLAYER) {
        player = attacker;
        monster = target;
        sprite = true;
    } else {
        player = target;
        monster = attacker;
        sprite = false;
    }

    const vector2d_t anchor = draw_combat_view(combat_view_anchor, player, monster, ascii_goblin, GOBLIN_HEIGHT, false);
    if (consume_ability_resource(attacker, ability)) {
        if (roll_hit(attacker->current_stats.dexterity, target->current_stats.dexterity)) {
            int damage_dealt = deal_damage(target, ability->damage_type, roll_damage(ability));

            draw_combat_view(combat_view_anchor, player, monster, ascii_goblin, GOBLIN_HEIGHT, sprite);

            memset(message, 0, sizeof(message));
            snprintf(message, sizeof(message), "%s uses %s and deals %d %s damage to %s!",
                     attacker->name,
                     ability->name,
                     damage_dealt,
                     damage_type_to_string(ability->damage_type),
                     target->name);
            draw_combat_log(anchor, message);
        } else {
            draw_combat_view(combat_view_anchor, player, monster, ascii_goblin, GOBLIN_HEIGHT, false);

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

void use_potion(character_t* player, const character_t* monster, potion_t* item) {
    const vector2d_t anchor = draw_combat_view(combat_view_anchor, player, monster, ascii_goblin, GOBLIN_HEIGHT, false);
    invoke_potion_effect(player, item);

    char message[MAX_STRING_LENGTH];
    snprintf(message, sizeof(message), "%s uses a %s potion, restoring %d %s!",
             player->name,
             item->name,
             item->value,
             potion_type_to_string(item->effectType));
    draw_combat_log(anchor, message);
}

ability_t* get_random_ability(const character_t* character) {
    const int random_index = rand() % character->ability_count;
    return character->abilities[random_index];
}

void invoke_potion_effect(character_t* character, potion_t* potion) {
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
void collect_ability_menu_options(ability_t* abilities[], const int count) {
    //clear the ability menu options
    for (int i = 0; i < MAX_ABILITY_LIMIT; i++) {
        memset(ability_menu_options[i], '0', sizeof(char) * MAX_STRING_LENGTH);
    }

    for (int i = 0; i < count; i++) {
        const char* ability_format = "%-16s Rolls: %-2d | Accuracy: %-3d%% | Cost: %-3d | Dice: %-4s | Type: %-16s";

        snprintf(ability_menu_options[i], MAX_STRING_LENGTH,
                 ability_format,
                 abilities[i]->name,
                 abilities[i]->roll_amount,
                 abilities[i]->accuracy,
                 abilities[i]->resource_cost,
                 dice_size_to_string(abilities[i]->dice_size),
                 damage_type_to_string(abilities[i]->damage_type));
    }
}

// Helper function to create potion options array
void collect_potion_menu_options(potion_t* potions[], const int count) {
    log_msg(FINE, "Combat Mode", "collect_potion_menu_options");
    // clear the potion menu options
    for (int i = 0; i < MAX_POTION_LIMIT; i++) {
        memset(potion_menu_options[i], 0, MAX_STRING_LENGTH);
    }
    log_msg(FINE, "Combat Mode", "Potion count: %d", count);


    for (int i = 0; i < count; i++) {
        const char* potion_format = "%-16s  Type: %-16s | Value: %-3d";

        snprintf(potion_menu_options[i], MAX_STRING_LENGTH,
                 potion_format,
                 potions[i]->name,
                 potion_type_to_string(potions[i]->effectType),
                 potions[i]->value);
    }
}

void update_local(void) {
    snprintf(combat_menu_options[0], MAX_STRING_LENGTH, "%s", get_local_string(MAIN_MENU_OPTION1_KEY));
    snprintf(combat_menu_options[1], MAX_STRING_LENGTH, "%s", get_local_string(MAIN_MENU_OPTION2_KEY));
}

void shutdown_combat_mode(void) {
    if (combat_menu_options != NULL) {
        for (int i = 0; i < MAX_COMBAT_MENU_OPTIONS; i++) {
            if (combat_menu_options[i] != NULL) free(combat_menu_options[i]);
        }
        free(combat_menu_options);
    }

    if (ability_menu_options != NULL) {
        for (int i = 0; i < MAX_ABILITY_LIMIT; i++) {
            if (ability_menu_options[i] != NULL) free(ability_menu_options[i]);
        }
        free(ability_menu_options);
    }

    if (potion_menu_options != NULL) {
        for (int i = 0; i < MAX_POTION_LIMIT; i++) {
            if (potion_menu_options[i] != NULL) free(potion_menu_options[i]);
        }
        free(potion_menu_options);
    }
}
