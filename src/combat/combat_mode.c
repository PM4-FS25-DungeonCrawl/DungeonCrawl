/**
 * @file combat_mode.c
 * @brief Implementation of the combat mode.
 */
#include "combat_mode.h"

#include "../character/character.h"
#include "../character/level.h"
#include "../common.h"
#include "../io/input/input_handler.h"
#include "../io/io_handler.h"
#include "../io/output/common/output_handler.h"
#include "../io/output/media/media_files.h"
#include "../io/output/specific/combat_output.h"
#include "../item/local/potion_local.h"
#include "../local/local_handler.h"
#include "ability.h"
#include "local/ability_local.h"
#include "local/combat_mode_local.h"

#include <stdlib.h>
#include <unistd.h>

// Internal functions
void collect_ability_menu_options(ability_t* abilities[], int count);
void collect_potion_menu_options(potion_t* potions[], int count);

// Internal global variables
vector2d_t combat_view_anchor = {1, 1};
internal_combat_state_t combat_state = COMBAT_MENU;

int ability_count = 0;
int potion_count = 0;

char** ability_menu_options = NULL;
char** potion_menu_options = NULL;

// Define internal functions

/** @brief Use an ability on a target character.
 * @param attacker Pointer to the attacker character.
 * @param target Pointer to the target character.
 * @param ability Pointer to the ability to be used.
 */
void use_ability(character_t* attacker, character_t* target, const ability_t* ability);
/** @brief Use a potion on a target character.
 * @param player Pointer to the player character.
 * @param monster Pointer to the monster character.
 * @param potion Pointer to the potion to be used.
 */
void use_potion(character_t* player, const character_t* monster, potion_t* potion);
/** @brief Consumes the mana or stamina resource of the attacker character.
 * @param attacker Pointer to the attacker character.
 * @param ability Pointer to the ability to be used.
 * @return true if the resource was consumed, false otherwise.
 */
bool consume_ability_resource(character_t* attacker, const ability_t* ability);
/** @brief Get a random ability from the character's abilities.
 * @param character Pointer to the character whose abilities are to be used.
 * @return Pointer to the randomly selected ability.
 */
ability_t* get_random_ability(const character_t* character);

/**
 * @brief Initialize the combat mode
 * @note This function must be called before using any other functions in this module.
 */
int init_combat_mode() {
    combat_mode_strings = (char**) malloc(MAX_COMBAT_MODE_STRINGS * sizeof(char*));
    RETURN_WHEN_NULL(combat_mode_strings, -1, "Combat Mode", "Allocated memory for combat mode strings in memory pool is NULL");

    ability_menu_options = (char**) malloc(sizeof(char*) * MAX_ABILITY_LIMIT);
    if (ability_menu_options == NULL) {
        free(combat_mode_strings);
        combat_mode_strings = NULL;
        log_msg(ERROR, "Combat Mode", "Failed to allocate memory for ability menu options.");
        return -1;
    }

    potion_menu_options = (char**) malloc(sizeof(char*) * MAX_POTION_LIMIT);
    if (potion_menu_options == NULL) {
        free(combat_mode_strings);
        combat_mode_strings = NULL;
        free(ability_menu_options);
        log_msg(ERROR, "Combat Mode", "Failed to allocate memory for potion menu options.");
        return -1;
    }

    for (int i = 0; i < MAX_COMBAT_MODE_STRINGS; i++) {
        combat_mode_strings[i] = NULL;
    }

    for (int i = 0; i < MAX_ABILITY_LIMIT; i++) {
        ability_menu_options[i] = NULL;
    }

    for (int i = 0; i < MAX_POTION_LIMIT; i++) {
        potion_menu_options[i] = NULL;
    }

    //update local once, so the strings are initialized
    update_combat_local();
    //add update local function to the observer list
    observe_local(update_combat_local);
    return 0;
}

combat_result_t start_combat(character_t* player, character_t* monster) {
    // initial combat state
    const vector2d_t anchor = draw_combat_view(combat_view_anchor, player, monster, GOBLIN_PNG, GOBLIN_HEIGHT, false);

    //collect menu options
    collect_ability_menu_options(player->abilities, player->ability_count);
    collect_potion_menu_options(player->potion_inventory, player->potion_count);

    switch (combat_state) {
        case COMBAT_MENU:
            combat_state = combat_menu(player, monster);
            break;
        case ABILITY_MENU:
            combat_state = ability_menu(player, monster);
            break;
        case POTION_MENU:
            combat_state = potion_menu(player, monster);
            break;
        case EVALUATE_COMBAT:
            // evaluate the combat result
            if (player->current_resources.health <= 0) {
                media_cleanup();
                draw_game_over();
                return PLAYER_LOST;
            }
            if (monster->current_resources.health <= 0) {
                clear_screen();
                media_cleanup();

                char message[64];
                snprintf(message, sizeof(message), "%s %s %s",
                         combat_mode_strings[WON_COMBAT_MSG1], monster->name, combat_mode_strings[WON_COMBAT_MSG2]);
                draw_combat_log(anchor, message);

                player->xp += monster->xp_reward;
                if (player->xp >= calculate_xp_for_next_level(player->level)) {
                    level_up(player);
                }
                return PLAYER_WON;
            }
            clear_screen();
            combat_state = COMBAT_MENU;
            break;
        case COMBAT_EXIT:
            media_cleanup();
            return EXIT_GAME;
    }
    return CONTINUE_COMBAT;
}

internal_combat_state_t combat_menu(const character_t* player, const character_t* monster) {
    // draw combat view
    const vector2d_t anchor = draw_combat_view(combat_view_anchor, player, monster, GOBLIN_PNG, GOBLIN_HEIGHT, false);
    int selected_index = 0;

    internal_combat_state_t new_state = COMBAT_MENU;
    bool submenu_selected = false;

    while (!submenu_selected) {
        // draw menu options
        draw_combat_menu(anchor,
                         combat_mode_strings[MAIN_MENU_TITLE],
                         &combat_mode_strings[MAIN_MENU_OPTION1],
                         2,
                         selected_index,
                         NULL);

        // check for input
        input_event_t input_event;
        if (!get_input_blocking(&input_event)) {
            continue;
        }

        // Handle input using logical input types
        switch (input_event.type) {
            case INPUT_UP:
                // Move up
                selected_index = (selected_index - 1 + 2) % 2;
                break;
            case INPUT_DOWN:
                // Move down
                selected_index = (selected_index + 1) % 2;
                break;
            case INPUT_CONFIRM:
                // Return the selected state
                if (selected_index == 0) {
                    new_state = ABILITY_MENU;
                } else if (selected_index == 1) {
                    new_state = POTION_MENU;
                }
                submenu_selected = true;
                break;
            case INPUT_QUIT:
                // Exit the game
                new_state = COMBAT_EXIT;
                submenu_selected = true;
                break;
            default:;
        }
    }
    return new_state;
}

internal_combat_state_t ability_menu(character_t* player, character_t* monster) {
    clear_screen();
    // draw combat view
    const vector2d_t anchor = draw_combat_view(combat_view_anchor, player, monster, GOBLIN_PNG, GOBLIN_HEIGHT, false);
    int selected_index = 0;

    internal_combat_state_t new_state = ABILITY_MENU;
    bool ability_used_or_esc = false;

    while (!ability_used_or_esc) {
        // draw menu options
        draw_combat_menu(anchor,
                         combat_mode_strings[ABILITY_MENU_TITLE],
                         ability_menu_options,
                         player->ability_count,
                         selected_index,
                         combat_mode_strings[PRESS_C_RETURN]);

        // check for input
        input_event_t input_event;
        if (!get_input_blocking(&input_event)) {
            continue;
        }

        // Handle input using logical input types
        switch (input_event.type) {
            case INPUT_UP:
                // Move up
                selected_index = (selected_index - 1 + player->ability_count) % player->ability_count;
                break;
            case INPUT_DOWN:
                // Move down
                selected_index = (selected_index + 1) % player->ability_count;
                break;
            case INPUT_CONFIRM:
                // Use ability
                use_ability(player, monster, player->abilities[selected_index]);
                use_ability(monster, player, get_random_ability(monster));

                new_state = EVALUATE_COMBAT;
                ability_used_or_esc = true;
                break;
            case INPUT_CANCEL:
                // go back to the combat menu
                new_state = COMBAT_MENU;
                ability_used_or_esc = true;
                break;
            default:;
        }
    }
    return new_state;
}

internal_combat_state_t potion_menu(character_t* player, character_t* monster) {
    // draw combat view
    const vector2d_t anchor = draw_combat_view(combat_view_anchor, player, monster, GOBLIN_PNG, GOBLIN_HEIGHT, false);
    int selected_index = 0;

    if (player->potion_count == 0) {
        draw_combat_log(anchor, combat_mode_strings[NO_MORE_POTIONS]);
        return COMBAT_MENU;
    }

    internal_combat_state_t new_state = POTION_MENU;
    bool item_used_or_esc = false;

    while (!item_used_or_esc) {
        // draw menu options
        draw_combat_menu(anchor,
                         combat_mode_strings[POTION_MENU_TITLE],
                         potion_menu_options,
                         player->potion_count,
                         selected_index,
                         combat_mode_strings[PRESS_C_RETURN]);

        // check for input
        input_event_t input_event;
        if (!get_input_blocking(&input_event)) {
            continue;
        }

        // Handle input using logical input types
        switch (input_event.type) {
            case INPUT_UP:
                // Move up
                selected_index = (selected_index - 1 + player->potion_count) % player->potion_count;
                break;
            case INPUT_DOWN:
                // Move down
                selected_index = (selected_index + 1) % player->potion_count;
                break;
            case INPUT_CONFIRM:
                // Use the selected potion
                use_potion(player, monster, player->potion_inventory[selected_index]);
                use_ability(monster, player, get_random_ability(monster));
                new_state = EVALUATE_COMBAT;

                collect_potion_menu_options(player->potion_inventory, player->potion_count);
                item_used_or_esc = true;
                break;
            case INPUT_CANCEL:
                // Go back to the combat menu
                new_state = COMBAT_MENU;
                item_used_or_esc = true;
                break;
            default:;
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

    const vector2d_t anchor = draw_combat_view(combat_view_anchor, player, monster, GOBLIN_PNG, GOBLIN_HEIGHT, false);
    if (consume_ability_resource(attacker, ability)) {
        if (roll_hit(attacker->current_stats.dexterity, target->current_stats.dexterity)) {
            const int damage_dealt = deal_damage(target, ability->damage_type, roll_damage(ability));

            draw_combat_view(combat_view_anchor, player, monster, GOBLIN_PNG, GOBLIN_HEIGHT, sprite);

            memset(message, 0, sizeof(message));
            snprintf(message, sizeof(message), combat_mode_strings[ATTACK_SUCCESS],//TODO: This Method of using formats is not safe!!
                     attacker->name,
                     ability_names[ability->id],
                     damage_dealt,
                     damage_type_to_string(ability->damage_type),
                     target->name);
            draw_combat_log(anchor, message);
        } else {
            draw_combat_view(combat_view_anchor, player, monster, GOBLIN_PNG, GOBLIN_HEIGHT, false);

            memset(message, 0, sizeof(message));
            snprintf(message, sizeof(message), combat_mode_strings[ATTACK_MISS],//TODO: This Method of using formats is not safe!!
                     attacker->name,
                     ability_names[ability->id]);
            draw_combat_log(anchor, message);
        }
    } else {
        memset(message, 0, sizeof(message));
        snprintf(message, sizeof(message), combat_mode_strings[ATTACK_FAIL],//TODO: This Method of using formats is not safe!!
                 attacker->name,
                 ability_names[ability->id]);
        draw_combat_log(anchor, message);
    }
    render_frame();
}

void use_potion(character_t* player, const character_t* monster, potion_t* potion) {
    const vector2d_t anchor = draw_combat_view(combat_view_anchor, player, monster, GOBLIN_PNG, GOBLIN_HEIGHT, false);
    invoke_potion_effect(player, potion);

    char message[MAX_STRING_LENGTH];
    snprintf(message, sizeof(message), combat_mode_strings[POTION_USE],//TODO: This Method of using formats is not safe!!
             player->name,
             potion_names[potion->effectType],
             potion->value,
             potion_type_to_string(potion->effectType));
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
        case MANA:
            if (potion->value > (character->max_resources.mana - character->current_resources.mana)) {
                character->current_resources.mana = character->max_resources.mana;
            } else {
                character->current_resources.mana += potion->value;
            }
            break;

        case STAMINA:
            if (potion->value > (character->max_resources.stamina - character->current_resources.stamina)) {
                character->current_resources.stamina = character->max_resources.stamina;
            } else {
                character->current_resources.stamina += potion->value;
            }
            break;

        default:
            log_msg(ERROR, "Character", "Unknown potion effect type: %d", potion->effectType);
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

/**
 * @brief Collects all the options for abilities in the abilities menu for displaying.
 *
 * @param abilities An array of abilities.
 * @param count Ammount of abilities in the array.
 */
void collect_ability_menu_options(ability_t* abilities[], const int count) {
    for (int i = 0; i < MAX_ABILITY_LIMIT; i++) {
        if (ability_menu_options[i] != NULL) {
            free(ability_menu_options[i]);
            ability_menu_options[i] = NULL;
        }
    }

    for (int i = 0; i < count; i++) {
        ability_menu_options[i] = (char*) malloc(MAX_STRING_LENGTH * sizeof(char));
        if (ability_menu_options[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(ability_menu_options[j]);
                ability_menu_options[j] = NULL;
            }
            log_msg(ERROR, "Combat Mode", "Failed to allocate memory for ability menu options.");
            return;
        }
    }

    for (int i = 0; i < count; i++) {
        snprintf(ability_menu_options[i], MAX_STRING_LENGTH,
                 combat_mode_strings[ABILITY_FORMAT],//TODO: This Method of using formats is not safe!!
                 ability_names[abilities[i]->id],
                 abilities[i]->roll_amount,
                 abilities[i]->accuracy,
                 abilities[i]->resource_cost,
                 dice_size_to_string(abilities[i]->dice_size),
                 damage_type_to_string(abilities[i]->damage_type));
    }
}

/**
 * @brief Collects all the options for potions in the potion menu for displaying.
 *
 * @param potions An array of potions.
 * @param count Ammount of potions in the array.
 */
void collect_potion_menu_options(potion_t* potions[], const int count) {
    for (int i = 0; i < MAX_POTION_LIMIT; i++) {
        if (potion_menu_options[i] != NULL) {
            free(potion_menu_options[i]);
            potion_menu_options[i] = NULL;
        }
    }

    for (int i = 0; i < count; i++) {
        potion_menu_options[i] = (char*) malloc(MAX_STRING_LENGTH * sizeof(char));
        if (potion_menu_options[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(potion_menu_options[j]);
                potion_menu_options[j] = NULL;
            }
            log_msg(ERROR, "Combat Mode", "Failed to allocate memory for potion menu options.");
            return;
        }
    }

    for (int i = 0; i < count; i++) {
        snprintf(potion_menu_options[i], MAX_STRING_LENGTH,
                 combat_mode_strings[POTION_FORMAT],//TODO: This Method of using formats is not safe!!
                 potion_names[potions[i]->effectType],
                 potion_type_to_string(potions[i]->effectType),
                 potions[i]->value);
    }
}

void shutdown_combat_mode() {
    if (combat_mode_strings != NULL) {
        for (int i = 0; i < MAX_COMBAT_MODE_STRINGS; i++) {
            if (combat_mode_strings[i] != NULL) {
                free(combat_mode_strings[i]);
                combat_mode_strings[i] = NULL;
            }
        }
        free(combat_mode_strings);
        combat_mode_strings = NULL;
    }

    if (ability_menu_options != NULL) {
        for (int i = 0; i < MAX_ABILITY_LIMIT; i++) {
            if (ability_menu_options[i] != NULL) {
                free(ability_menu_options[i]);
                ability_menu_options[i] = NULL;
            }
        }
        free(ability_menu_options);
        ability_menu_options = NULL;
    }

    if (potion_menu_options != NULL) {
        for (int i = 0; i < MAX_POTION_LIMIT; i++) {
            if (potion_menu_options[i] != NULL) {
                free(potion_menu_options[i]);
                potion_menu_options[i] = NULL;
            }
        }
        free(potion_menu_options);
        potion_menu_options = NULL;
    }
}
