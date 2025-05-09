#include "combat_mode.h"

#include "../asciiart/ascii.h"
#include "../character/character.h"
#include "../character/level.h"
#include "../common.h"
#include "../game.h"
#include "../local/local.h"
#include "../local/local_strings.h"
#include "ability.h"
#include "draw/draw_combat_mode.h"

#include <notcurses/notcurses.h>
#include <stdbool.h>
#include <stdint.h>


#ifdef __APPLE__
    #define KEY_EVENT NCTYPE_PRESS
#else
    #define KEY_EVENT NCTYPE_UNKNOWN
#endif /* ifdef __APPLE__ */


// Internal functions
void invoke_potion_effect(character_t* character, potion_t* potion);

void collect_ability_menu_options(ability_t* abilities[], int count);
void collect_potion_menu_options(potion_t* potions[], int count);

/**
 * @brief Updates the localized strings used in the combat mode menus.
 *
 * This function populates the localized string buffers with values retrieved
 * from the localization system. These strings are used in various menus within
 * the combat mode, such as the main menu, ability menu, and potion menu. It also
 * updates messages like the tail message and combat-specific messages.
 *
 * @note This function is automatically called during initialization of combat
 * mode to ensure that all strings are properly localized before use. It is
 * also registered as an observer to keep menu strings updated whenever the locale changes.
 */
void update_combat_local(void);

// Internal global variables
vector2d_t combat_view_anchor = {1, 1};
internal_combat_state_t combat_state = COMBAT_MENU;

string_max_t* ability_menu_options;// holds the ability menu options
string_max_t* potion_menu_options; // holds the potion menu options

int init_combat_mode() {
    ability_menu_options = memory_pool_alloc(main_memory_pool, sizeof(string_max_t) * MAX_ABILITY_LIMIT);
    NULL_PTR_HANDLER_RETURN(ability_menu_options, -1, "Combat Mode", "Allocated memory for ability menu options in memory pool is NULL");

    potion_menu_options = memory_pool_alloc(main_memory_pool, sizeof(string_max_t) * MAX_POTION_LIMIT);
    NULL_PTR_HANDLER_RETURN(potion_menu_options, -1, "Combat Mode", "Allocated memory for potion menu options in memory pool is NULL");

    //update local once, so the strings are initialized
    update_combat_local();
    //add update local function to the observer list
    add_local_observer(update_combat_local);
    return 0;
}

combat_result_t start_combat(character_t* player, character_t* monster) {
    // initial combat state
    const vector2d_t anchor = draw_combat_view(combat_view_anchor, player, monster, ascii_goblin, GOBLIN_HEIGHT, false);

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
                return PLAYER_LOST;
                draw_game_over();
            } else if (monster->current_resources.health <= 0) {
                return PLAYER_WON;
                // clear screen
                ncplane_set_channels(stdplane, DEFAULT_COLORS);
                for (uint i = 0; i < ncplane_dim_x(stdplane); i++) {
                    for (uint j = 0; j < ncplane_dim_y(stdplane); j++) {
                        ncplane_printf_yx(stdplane, (int) j, (int) i, " ");
                    }
                }
                char message[MAX_STRING_LENGTH];
                snprintf(message, sizeof(message), "You won the combat! %s is dead.", monster->name);
                draw_combat_log(anchor, message);
                player->xp += monster->xp_reward;
                if (player->xp >= calculate_xp_for_next_level(player->level)) {
                    level_up(player);
                }
            } else {
                combat_state = COMBAT_MENU;
            }
            break;
        case COMBAT_EXIT:
            return EXIT_GAME;
            break;
    }
    return CONTINUE_COMBAT;
}

internal_combat_state_t combat_menu(const character_t* player, const character_t* monster) {
    // draw combat view
    const vector2d_t anchor = draw_combat_view(combat_view_anchor, player, monster, ascii_goblin, GOBLIN_HEIGHT, false);
    int selected_index = 0;

    internal_combat_state_t new_state = COMBAT_MENU;
    bool submenu_selected = false;

    while (!submenu_selected) {
        // draw menu options
        draw_combat_menu(anchor,
                         local_strings[como_main_menu_title.idx].characters,
                         &local_strings[como_main_menu_option1.idx],
                         MAX_COMO_MAIN_MENU_OPTION,
                         selected_index,
                         NULL);

        // check for input
        ncinput event;

        memset(&event, 0, sizeof(event));
        notcurses_get_blocking(nc, &event);

        // skip if key event is release
        if (!(event.evtype == NCTYPE_UNKNOWN || event.evtype == NCTYPE_PRESS)) { continue; }
        if (event.id == NCKEY_UP) {
            // Move up
            selected_index = (selected_index - 1 + MAX_COMO_MAIN_MENU_OPTION) % MAX_COMO_MAIN_MENU_OPTION;
        } else if (event.id == NCKEY_DOWN) {
            // Move down
            selected_index = (selected_index + 1) % MAX_COMO_MAIN_MENU_OPTION;
        } else if (event.id == NCKEY_ENTER) {
            // Return the selected state
            if (selected_index == 0) {
                new_state = ABILITY_MENU;
            } else if (selected_index == 1) {
                new_state = POTION_MENU;
            }
            submenu_selected = true;
        } else if (event.id == 'c' && (event.modifiers & NCKEY_MOD_CTRL)) {
            // Exit the game
            new_state = COMBAT_EXIT;
            submenu_selected = true;
        }
    }
    return new_state;
}

internal_combat_state_t ability_menu(character_t* player, character_t* monster) {
    // Clear the screen before drawing a new menu
    ncplane_set_channels(stdplane, DEFAULT_COLORS);
    for (uint i = 0; i < ncplane_dim_x(stdplane); i++) {
        for (uint j = 0; j < ncplane_dim_y(stdplane); j++) {
            ncplane_printf_yx(stdplane, (int) j, (int) i, " ");
        }
    }
    // draw combat view
    const vector2d_t anchor = draw_combat_view(combat_view_anchor, player, monster, ascii_goblin, GOBLIN_HEIGHT, false);
    int selected_index = 0;

    internal_combat_state_t new_state = ABILITY_MENU;
    bool ability_used_or_esc = false;

    while (!ability_used_or_esc) {
        // draw menu options
        draw_combat_menu(anchor,
                         local_strings[como_ability_menu_title.idx].characters,
                         ability_menu_options,
                         player->ability_count,
                         selected_index,
                         local_strings[como_submenu_tail_message.idx].characters);

        // check for input
        ncinput event;
        memset(&event, 0, sizeof(event));
        notcurses_get_blocking(nc, &event);

        // skip if key event is release
        if (!(event.evtype == NCTYPE_UNKNOWN || event.evtype == NCTYPE_PRESS)) { continue; }

        if (event.id == NCKEY_UP) {
            // Move up
            selected_index = (selected_index - 1 + player->ability_count) % player->ability_count;
        } else if (event.id == NCKEY_DOWN) {
            // Move down
            selected_index = (selected_index + 1) % player->ability_count;
        } else if (event.id == NCKEY_ENTER) {
            use_ability(player, monster, player->abilities[selected_index]);
            use_ability(monster, player, get_random_ability(monster));

            new_state = EVALUATE_COMBAT;
            ability_used_or_esc = true;
        } else if (event.id == NCKEY_ESC) {
            // go back to the combat menu
            new_state = COMBAT_MENU;
            ability_used_or_esc = true;
        }
    }
    return new_state;
}

internal_combat_state_t potion_menu(character_t* player, character_t* monster) {
    // draw combat view
    const vector2d_t anchor = draw_combat_view(combat_view_anchor, player, monster, ascii_goblin, GOBLIN_HEIGHT, false);
    int selected_index = 0;

    if (player->potion_count == 0) {
        draw_combat_log(anchor, local_strings[como_no_more_potions.idx].characters);
        return COMBAT_MENU;
    }

    internal_combat_state_t new_state = POTION_MENU;
    bool item_used_or_esc = false;

    while (!item_used_or_esc) {
        // draw menu options
        draw_combat_menu(anchor,
                         local_strings[como_potion_menu_title.idx].characters,
                         potion_menu_options,
                         player->potion_count,
                         selected_index,
                         local_strings[como_submenu_tail_message.idx].characters);

        // check for input
        ncinput event;
        memset(&event, 0, sizeof(event));
        notcurses_get_blocking(nc, &event);

        if (!(event.evtype == NCTYPE_UNKNOWN || event.evtype == NCTYPE_PRESS)) { continue; }
        if (event.id == NCKEY_UP) {
            // Move up
            selected_index = (selected_index - 1 + player->potion_count) % player->potion_count;
        } else if (event.id == NCKEY_DOWN) {
            // Move down
            selected_index = (selected_index + 1) % player->potion_count;
        } else if (event.id == NCKEY_ENTER) {
            // Use the selected potion
            use_potion(player, monster, player->potion_inventory[selected_index]);
            use_ability(monster, player, get_random_ability(monster));
            new_state = EVALUATE_COMBAT;

            collect_potion_menu_options(player->potion_inventory, player->potion_count);
            item_used_or_esc = true;
        } else if (event.id == NCKEY_ESC) {
            // Go back to the combat menu
            new_state = COMBAT_MENU;
            item_used_or_esc = true;
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
            const int damage_dealt = deal_damage(target, ability->damage_type, roll_damage(ability));

            draw_combat_view(combat_view_anchor, player, monster, ascii_goblin, GOBLIN_HEIGHT, sprite);


            memset(message, 0, sizeof(message));
            snprintf(message, sizeof(message), local_strings[como_attack_success.idx].characters,//TODO: This Method of using formats is not safe!!
                     attacker->name,
                     ability->name,
                     damage_dealt,
                     damage_type_to_string(ability->damage_type),
                     target->name);
            draw_combat_log(anchor, message);
        } else {
            draw_combat_view(combat_view_anchor, player, monster, ascii_goblin, GOBLIN_HEIGHT, false);

            memset(message, 0, sizeof(message));
            snprintf(message, sizeof(message), local_strings[como_attack_miss.idx].characters,//TODO: This Method of using formats is not safe!!
                     attacker->name,
                     ability->name);
            draw_combat_log(anchor, message);
        }
    } else {
        memset(message, 0, sizeof(message));
        snprintf(message, sizeof(message), local_strings[como_attack_fail.idx].characters,//TODO: This Method of using formats is not safe!!
                 attacker->name,
                 ability->name);
        draw_combat_log(anchor, message);
    }
    notcurses_render(nc);
}

void use_potion(character_t* player, const character_t* monster, potion_t* potion) {
    const vector2d_t anchor = draw_combat_view(combat_view_anchor, player, monster, ascii_goblin, GOBLIN_HEIGHT, false);
    invoke_potion_effect(player, potion);

    char message[MAX_STRING_LENGTH];
    snprintf(message, sizeof(message), local_strings[como_potion_use.idx].characters,//TODO: This Method of using formats is not safe!!
             player->name,
             potion->name,
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

// Helper function to create ability options array
void collect_ability_menu_options(ability_t* abilities[], const int count) {
    //clear the ability menu options
    ncplane_set_channels(stdplane, DEFAULT_COLORS);
    for (int i = 0; i < MAX_ABILITY_LIMIT; i++) {
        memset(ability_menu_options[i].characters, '\0', sizeof(char) * MAX_STRING_LENGTH);
    }

    for (int i = 0; i < count; i++) {
        snprintf(ability_menu_options[i].characters, MAX_STRING_LENGTH,
                 local_strings[como_ability_format.idx].characters,//TODO: This Method of using formats is not safe!!
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
    // clear the potion menu options
    ncplane_set_channels(stdplane, DEFAULT_COLORS);
    for (int i = 0; i < MAX_POTION_LIMIT; i++) {
        memset(potion_menu_options[i].characters, '\0', MAX_STRING_LENGTH);
    }

    for (int i = 0; i < count; i++) {
        snprintf(potion_menu_options[i].characters, MAX_STRING_LENGTH,
                 local_strings[como_potion_format.idx].characters,//TODO: This Method of using formats is not safe!!
                 potions[i]->name,
                 potion_type_to_string(potions[i]->effectType),
                 potions[i]->value);
    }
}


void update_combat_local(void) {
    //main menu
    snprintf(local_strings[como_main_menu_title.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_main_menu_title.key));
    snprintf(local_strings[como_main_menu_option1.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_main_menu_option1.key));
    snprintf(local_strings[como_main_menu_option2.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_main_menu_option2.key));

    //ability menu
    snprintf(local_strings[como_ability_menu_title.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_ability_menu_title.key));
    snprintf(local_strings[como_ability_format.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_ability_format.key));

    //potion menu
    snprintf(local_strings[como_potion_menu_title.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_potion_menu_title.key));
    snprintf(local_strings[como_potion_format.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_potion_format.key));

    //tail message
    snprintf(local_strings[como_submenu_tail_message.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_submenu_tail_message.key));

    //combat messages
    snprintf(local_strings[como_no_more_potions.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_no_more_potions.key));
    snprintf(local_strings[como_attack_success.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_attack_success.key));
    snprintf(local_strings[como_attack_miss.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_attack_miss.key));
    snprintf(local_strings[como_attack_fail.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_attack_fail.key));
    snprintf(local_strings[como_potion_use.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_potion_use.key));
}

void shutdown_combat_mode() {
    memory_pool_free(main_memory_pool, ability_menu_options);
    memory_pool_free(main_memory_pool, potion_menu_options);
}
