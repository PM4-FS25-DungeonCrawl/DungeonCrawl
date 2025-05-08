#include "combat_output.h"
#include "../colors.h"
#include "../common/common_output.h"
#include "../media/media_output.h"
#include "../../input/input_handler.h"
#include "../../menu/menu.h"
#include "../../../logging/logger.h"
#include "../../../local/local.h"  // For localization
#include "../../../character/character.h"
#include "../../../combat/ability.h"
#include "../../../combat/damage.h"

#include <stdio.h>
#include <string.h>

// Define constants for combat rendering
#define PLAYER_X 10
#define PLAYER_Y 12
#define MONSTER_X 50
#define MONSTER_Y 12
#define LOG_WIDTH 70
#define LOG_HEIGHT 6

// Combat log buffer
static char combat_log[LOG_HEIGHT][256];
static int current_log_line = 0;

// Forward declarations of helper functions
static void add_to_combat_log(const char* message);
static void render_health_bar(int x, int y, int current, int max, int width);

bool init_combat_output(void) {
    // Initialize the combat log
    for (int i = 0; i < LOG_HEIGHT; i++) {
        combat_log[i][0] = '\0';
    }
    current_log_line = 0;
    
    log_msg(INFO, "combat_output", "Combat output handler initialized");
    return true;
}

bool render_combat_screen(const character_t* player, const character_t* monster) {
    if (!player || !monster) {
        log_msg(ERROR, "combat_output", "Null player or monster pointer");
        return false;
    }
    
    // Get screen dimensions for centering
    int screen_width, screen_height;
    if (!get_screen_dimensions(&screen_width, &screen_height)) {
        log_msg(ERROR, "combat_output", "Failed to get screen dimensions");
        return false;
    }
    
    // Clear the screen
    clear_screen(COLOR_UI_BG);
    
    // Draw the combat area
    int combat_width = 80;
    int combat_height = 25;
    int combat_x = (screen_width - combat_width) / 2;
    int combat_y = (screen_height - combat_height) / 2;
    
    draw_box(combat_y, combat_x, combat_height, combat_width, "Combat", COLOR_UI_FG, COLOR_UI_BG);
    
    // Draw player
    int player_x = combat_x + PLAYER_X;
    int player_y = combat_y + PLAYER_Y;
    
    draw_box(player_y, player_x, 5, 20, player->name, COLOR_PLAYER, COLOR_UI_BG);
    
    // Draw player health and mana bars
    render_health_bar(player_x + 1, player_y + 1, player->current_resources.health, 
                     player->max_resources.health, 18);
    render_health_bar(player_x + 1, player_y + 2, player->current_resources.mana, 
                     player->max_resources.mana, 18);
    
    // Add a little player icon (in a real game this could be a character visual)
    print_text(player_y + 3, player_x + 9, "@", COLOR_PLAYER, COLOR_UI_BG);
    
    // Draw monster
    int monster_x = combat_x + MONSTER_X;
    int monster_y = combat_y + MONSTER_Y;
    
    draw_box(monster_y, monster_x, 5, 20, monster->name, COLOR_MONSTER, COLOR_UI_BG);
    
    // Draw monster health and mana bars
    render_health_bar(monster_x + 1, monster_y + 1, monster->current_resources.health, 
                     monster->max_resources.health, 18);
    render_health_bar(monster_x + 1, monster_y + 2, monster->current_resources.mana, 
                     monster->max_resources.mana, 18);
    
    // Add a monster icon (in a real game this could be a monster visual)
    print_text(monster_y + 3, monster_x + 9, "M", COLOR_MONSTER, COLOR_UI_BG);
    
    // Draw versus symbol between player and monster
    print_text(combat_y + PLAYER_Y + 2, combat_x + 35, "VS", COLOR_UI_HIGHLIGHT, COLOR_UI_BG);
    
    // Draw combat log
    int log_x = combat_x + (combat_width - LOG_WIDTH) / 2;
    int log_y = combat_y + combat_height - LOG_HEIGHT - 2;
    
    draw_box(log_y, log_x, LOG_HEIGHT + 2, LOG_WIDTH, "Combat Log", COLOR_UI_FG, COLOR_UI_BG);
    
    // Draw log entries
    for (int i = 0; i < LOG_HEIGHT; i++) {
        int line_index = (current_log_line + i) % LOG_HEIGHT;
        if (combat_log[line_index][0] != '\0') {
            print_text(log_y + i + 1, log_x + 1, combat_log[line_index], COLOR_UI_FG, COLOR_UI_BG);
        }
    }
    
    // Draw controls help
    print_text(combat_y + combat_height - 1, combat_x + 4, 
              "Controls: A - Attack | S - Special | P - Potion | F - Flee", 
              COLOR_UI_FG, COLOR_UI_BG);
    
    // Render the frame
    render_frame();
    
    return true;
}

bool display_damage(const character_t* target, int damage, bool is_player) {
    if (!target) {
        log_msg(ERROR, "combat_output", "Null target pointer");
        return false;
    }
    
    // Get screen dimensions for centering
    int screen_width, screen_height;
    get_screen_dimensions(&screen_width, &screen_height);
    
    // Calculate position for damage display
    int combat_x = (screen_width - 80) / 2;
    int combat_y = (screen_height - 25) / 2;
    
    int x = combat_x + (is_player ? PLAYER_X : MONSTER_X) + 9;
    int y = combat_y + (is_player ? PLAYER_Y : MONSTER_Y) - 2;
    
    // Format damage text
    char damage_text[16];
    snprintf(damage_text, sizeof(damage_text), "-%d", damage);
    
    // Display damage in red
    print_text(y, x, damage_text, COLOR_RED, COLOR_UI_BG);
    
    // Add to combat log
    char log_message[256];
    snprintf(log_message, sizeof(log_message), "%s takes %d damage!", 
             target->name, damage);
    add_to_combat_log(log_message);
    
    // Render the frame
    render_frame();
    
    // Briefly pause to show the damage
    #ifdef _WIN32
        Sleep(500);  // 500 milliseconds
    #else
        usleep(500000);  // 500,000 microseconds = 500 milliseconds
    #endif
    
    return true;
}

bool display_ability(const ability_t* ability, const character_t* attacker, 
                   const character_t* target, int damage) {
    if (!ability || !attacker || !target) {
        log_msg(ERROR, "combat_output", "Null ability, attacker, or target pointer");
        return false;
    }
    
    // Get screen dimensions for centering
    int screen_width, screen_height;
    get_screen_dimensions(&screen_width, &screen_height);
    
    // Calculate position for ability effect display
    int combat_x = (screen_width - 80) / 2;
    int combat_y = (screen_height - 25) / 2;
    
    bool is_player_attacker = (strcmp(attacker->name, "Player") == 0);
    int start_x = combat_x + (is_player_attacker ? PLAYER_X : MONSTER_X) + 9;
    int start_y = combat_y + (is_player_attacker ? PLAYER_Y : MONSTER_Y) + 2;
    
    int end_x = combat_x + (is_player_attacker ? MONSTER_X : PLAYER_X) + 9;
    int end_y = combat_y + (is_player_attacker ? MONSTER_Y : PLAYER_Y) + 2;
    
    // Determine ability color based on type
    uint32_t ability_color;
    switch (ability->damage_type) {
        case DAMAGE_PHYSICAL:
            ability_color = COLOR_RED;
            break;
        case DAMAGE_MAGICAL:
            ability_color = COLOR_BLUE;
            break;
        default:
            ability_color = COLOR_YELLOW;
            break;
    }
    
    // Simple animation showing the ability path
    int mid_x = (start_x + end_x) / 2;
    int mid_y = (start_y + end_y) / 2;
    
    // Draw ability name at midpoint
    print_text(mid_y, mid_x - strlen(ability->name) / 2, ability->name, ability_color, COLOR_UI_BG);
    render_frame();
    
    // Pause briefly
    #ifdef _WIN32
        Sleep(300);  // 300 milliseconds
    #else
        usleep(300000);  // 300,000 microseconds = 300 milliseconds
    #endif
    
    // Show damage if applicable
    if (damage > 0) {
        display_damage(target, damage, !is_player_attacker);
    }
    
    // Add to combat log
    char log_message[256];
    snprintf(log_message, sizeof(log_message), "%s uses %s for %d damage!", 
             attacker->name, ability->name, damage);
    add_to_combat_log(log_message);
    
    return true;
}

combat_menu_result_t show_combat_menu(const character_t* player, const character_t* monster) {
    if (!player || !monster) {
        log_msg(ERROR, "combat_output", "Null player or monster pointer");
        return COMBAT_MENU_EXIT;  // Default to exit on error
    }
    
    // Get screen dimensions for centering
    int screen_width, screen_height;
    get_screen_dimensions(&screen_width, &screen_height);
    
    // Create menu options
    const char* options[] = {
        "Attack",
        "Use Ability",
        "Use Potion",
        "Flee"
    };
    
    int option_count = sizeof(options) / sizeof(options[0]);
    
    // Calculate menu position
    int combat_x = (screen_width - 80) / 2;
    int combat_y = (screen_height - 25) / 2;
    
    int menu_x = combat_x + 8;
    int menu_y = combat_y + 8;
    
    // Create the menu
    menu_t* menu = menu_create("Combat Actions", options, option_count, menu_x, menu_y);
    if (!menu) {
        log_msg(ERROR, "combat_output", "Failed to create combat menu");
        return COMBAT_MENU_EXIT;  // Default to exit on error
    }
    
    // Set menu colors
    menu_set_colors(menu,
                   COLOR_UI_HIGHLIGHT, COLOR_UI_BG,
                   COLOR_UI_FG, COLOR_UI_BG,
                   COLOR_UI_HIGHLIGHT, COLOR_UI_BG,
                   COLOR_WALL, COLOR_UI_BG);
    
    // Run the menu
    int selected_index;
    menu_result_t result = menu_run(menu, &selected_index, NULL);
    
    // Free the menu
    menu_free(menu);
    
    // Determine the result
    combat_menu_result_t combat_menu_result;
    
    switch (result) {
        case MENU_RESULT_SELECTED:
            // Map the selected index to the corresponding result
            switch (selected_index) {
                case 0:  // Attack
                    combat_menu_result = COMBAT_MENU_CONTINUE;
                    
                    // Add to combat log
                    add_to_combat_log("Player attacks!");
                    
                    break;
                case 1:  // Use Ability
                    combat_menu_result = COMBAT_MENU_ABILITY;
                    break;
                case 2:  // Use Potion
                    combat_menu_result = COMBAT_MENU_POTION;
                    break;
                case 3:  // Flee
                    combat_menu_result = COMBAT_MENU_EXIT;
                    
                    // Add to combat log
                    add_to_combat_log("Player attempts to flee!");
                    
                    break;
                default:
                    combat_menu_result = COMBAT_MENU_CONTINUE;
                    break;
            }
            break;
            
        case MENU_RESULT_CANCELED:
        case MENU_RESULT_EXIT:
        case MENU_RESULT_ERROR:
        default:
            combat_menu_result = COMBAT_MENU_EXIT;
            break;
    }
    
    // Re-render the combat screen to clear the menu
    render_combat_screen(player, monster);
    
    return combat_menu_result;
}

void shutdown_combat_output(void) {
    log_msg(INFO, "combat_output", "Combat output handler shut down");
}

// Helper function to add a message to the combat log
static void add_to_combat_log(const char* message) {
    if (!message) {
        return;
    }
    
    // Copy the message to the current log line
    strncpy(combat_log[current_log_line], message, 255);
    combat_log[current_log_line][255] = '\0';  // Ensure null termination
    
    // Move to the next log line
    current_log_line = (current_log_line + 1) % LOG_HEIGHT;
}

// Helper function to render a health/mana bar
static void render_health_bar(int x, int y, int current, int max, int width) {
    if (max <= 0) {
        return;
    }
    
    // Calculate the filled portion of the bar
    int filled = (current * (width - 2)) / max;
    if (filled < 0) filled = 0;
    if (filled > width - 2) filled = width - 2;
    
    // Draw the bar outline
    print_text(y, x, "[", COLOR_UI_FG, COLOR_UI_BG);
    print_text(y, x + width - 1, "]", COLOR_UI_FG, COLOR_UI_BG);
    
    // Draw the filled portion
    for (int i = 0; i < filled; i++) {
        print_text(y, x + i + 1, "=", COLOR_RED, COLOR_UI_BG);
    }
    
    // Draw the empty portion
    for (int i = filled; i < width - 2; i++) {
        print_text(y, x + i + 1, " ", COLOR_UI_FG, COLOR_UI_BG);
    }
    
    // Draw the numerical display
    char value_str[16];
    snprintf(value_str, sizeof(value_str), "%d/%d", current, max);
    
    int value_x = x + (width - strlen(value_str)) / 2;
    print_text(y, value_x, value_str, COLOR_WHITE, COLOR_UI_BG);
}