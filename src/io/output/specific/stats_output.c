#include "stats_output.h"
#include "../colors.h"
#include "../common/common_output.h"
#include "../../input/input_handler.h"
#include "../../../logging/logger.h"
#include "../../../local/local.h"  // For localization
#include "../../../character/character.h"
#include "../../../stats/stats.h"

#include <stdio.h>
#include <string.h>

bool init_stats_output(void) {
    log_msg(INFO, "stats_output", "Stats output handler initialized");
    return true;
}

bool render_stats_screen(const character_t* player) {
    if (!player) {
        log_msg(ERROR, "stats_output", "Null player pointer");
        return false;
    }
    
    // Get screen dimensions for centering
    int screen_width, screen_height;
    if (!get_screen_dimensions(&screen_width, &screen_height)) {
        log_msg(ERROR, "stats_output", "Failed to get screen dimensions");
        return false;
    }
    
    // Clear the screen
    clear_screen(COLOR_UI_BG);
    
    // Draw the stats window
    int window_width = 50;
    int window_height = 22;
    int window_x = (screen_width - window_width) / 2;
    int window_y = (screen_height - window_height) / 2;
    
    draw_box(window_y, window_x, window_height, window_width, "Character Stats", COLOR_UI_FG, COLOR_UI_BG);
    
    // Draw character basic info
    int text_x = window_x + 4;
    int text_y = window_y + 2;
    
    char level_str[32];
    char xp_str[32];
    snprintf(level_str, sizeof(level_str), "Level: %d", player->level);
    snprintf(xp_str, sizeof(xp_str), "Experience: %d / %d", player->experience, player->experience_to_next_level);
    
    print_text(text_y, text_x, level_str, COLOR_UI_HIGHLIGHT, COLOR_UI_BG);
    print_text(text_y + 1, text_x, xp_str, COLOR_UI_FG, COLOR_UI_BG);
    
    // Draw resources
    char health_str[32];
    char mana_str[32];
    snprintf(health_str, sizeof(health_str), "Health: %d / %d", 
             player->current_resources.health, player->max_resources.health);
    snprintf(mana_str, sizeof(mana_str), "Mana: %d / %d", 
             player->current_resources.mana, player->max_resources.mana);
    
    print_text(text_y + 3, text_x, health_str, COLOR_RED, COLOR_UI_BG);
    print_text(text_y + 4, text_x, mana_str, COLOR_BLUE, COLOR_UI_BG);
    
    // Draw statistics header
    print_text(text_y + 6, text_x, "Statistics", COLOR_UI_HIGHLIGHT, COLOR_UI_BG);
    
    // Draw base stats
    char str_str[32];
    char int_str[32];
    char dex_str[32];
    char con_str[32];
    
    snprintf(str_str, sizeof(str_str), "Strength:     %d", player->base_stats.strength);
    snprintf(int_str, sizeof(int_str), "Intelligence: %d", player->base_stats.intelligence);
    snprintf(dex_str, sizeof(dex_str), "Dexterity:    %d", player->base_stats.dexterity);
    snprintf(con_str, sizeof(con_str), "Constitution: %d", player->base_stats.constitution);
    
    print_text(text_y + 8, text_x, str_str, COLOR_UI_FG, COLOR_UI_BG);
    print_text(text_y + 9, text_x, int_str, COLOR_UI_FG, COLOR_UI_BG);
    print_text(text_y + 10, text_x, dex_str, COLOR_UI_FG, COLOR_UI_BG);
    print_text(text_y + 11, text_x, con_str, COLOR_UI_FG, COLOR_UI_BG);
    
    // Draw combat stats
    char phys_atk_str[32];
    char mag_atk_str[32];
    char phys_def_str[32];
    char mag_def_str[32];
    
    snprintf(phys_atk_str, sizeof(phys_atk_str), "Physical Attack:  %d", 
             calculate_physical_attack(&player->base_stats));
    snprintf(mag_atk_str, sizeof(mag_atk_str), "Magical Attack:   %d", 
             calculate_magical_attack(&player->base_stats));
    snprintf(phys_def_str, sizeof(phys_def_str), "Physical Defense: %d", 
             calculate_physical_defense(&player->base_stats));
    snprintf(mag_def_str, sizeof(mag_def_str), "Magical Defense:  %d", 
             calculate_magical_defense(&player->base_stats));
    
    print_text(text_y + 13, text_x, "Combat Stats", COLOR_UI_HIGHLIGHT, COLOR_UI_BG);
    print_text(text_y + 15, text_x, phys_atk_str, COLOR_UI_FG, COLOR_UI_BG);
    print_text(text_y + 16, text_x, mag_atk_str, COLOR_UI_FG, COLOR_UI_BG);
    print_text(text_y + 17, text_x, phys_def_str, COLOR_UI_FG, COLOR_UI_BG);
    print_text(text_y + 18, text_x, mag_def_str, COLOR_UI_FG, COLOR_UI_BG);
    
    // Draw skill points if available
    if (player->skill_points > 0) {
        char skill_points_str[64];
        snprintf(skill_points_str, sizeof(skill_points_str), 
                 "Available Skill Points: %d (Press ENTER to allocate)", player->skill_points);
        print_text(window_y + window_height - 2, window_x + 4, skill_points_str, 
                  COLOR_UI_HIGHLIGHT, COLOR_UI_BG);
    } else {
        print_text(window_y + window_height - 2, window_x + 4, 
                  "Press any key to return", COLOR_UI_FG, COLOR_UI_BG);
    }
    
    // Render the frame
    render_frame();
    
    return true;
}

// Helper function to show the stat allocation menu
static bool show_stat_allocation_menu(character_t* player) {
    if (!player || player->skill_points <= 0) {
        return false;
    }
    
    // Get screen dimensions for centering
    int screen_width, screen_height;
    get_screen_dimensions(&screen_width, &screen_height);
    
    // Create the stat options
    const char* options[] = {
        "Strength",
        "Intelligence",
        "Dexterity",
        "Constitution",
        "Cancel"
    };
    
    // Create the menu
    int menu_width = 20;
    int menu_x = (screen_width - menu_width) / 2;
    int menu_y = (screen_height - 7) / 2;  // 5 options + 2 for title and spacing
    
    // Draw the menu box
    draw_box(menu_y - 1, menu_x - 2, 8, menu_width + 4, "Allocate Skill Point", COLOR_UI_FG, COLOR_UI_BG);
    
    // Draw the options
    int selected = 0;
    bool allocating = true;
    
    while (allocating && player->skill_points > 0) {
        // Update the menu highlighting
        for (int i = 0; i < 5; i++) {
            uint32_t fg = (i == selected) ? COLOR_UI_HIGHLIGHT : COLOR_UI_FG;
            print_text(menu_y + i + 1, menu_x, options[i], fg, COLOR_UI_BG);
        }
        
        // Render the frame
        render_frame();
        
        // Get input
        input_event_t event;
        if (!get_input_blocking(&event)) {
            continue;
        }
        
        // Handle input
        switch (event.type) {
            case INPUT_UP:
                selected = (selected > 0) ? selected - 1 : 4;
                break;
                
            case INPUT_DOWN:
                selected = (selected < 4) ? selected + 1 : 0;
                break;
                
            case INPUT_CONFIRM:
                // Allocate the skill point
                if (selected < 4) {  // Not "Cancel"
                    switch (selected) {
                        case 0:  // Strength
                            player->base_stats.strength++;
                            break;
                        case 1:  // Intelligence
                            player->base_stats.intelligence++;
                            break;
                        case 2:  // Dexterity
                            player->base_stats.dexterity++;
                            break;
                        case 3:  // Constitution
                            player->base_stats.constitution++;
                            break;
                        default:
                            break;
                    }
                    
                    // Update derived stats
                    update_character_resources(&player->max_resources, &player->base_stats);
                    
                    // Decrease skill points
                    player->skill_points--;
                    
                    // Return to stats screen if no more points
                    if (player->skill_points <= 0) {
                        allocating = false;
                    }
                } else {
                    // Cancel
                    allocating = false;
                }
                break;
                
            case INPUT_CANCEL:
                allocating = false;
                break;
                
            default:
                break;
        }
    }
    
    // Re-render the stats screen
    render_stats_screen(player);
    
    return true;
}

bool handle_stats_input(character_t* player) {
    if (!player) {
        log_msg(ERROR, "stats_output", "Null player pointer");
        return false;
    }
    
    // Get input
    input_event_t event;
    if (!get_input_blocking(&event)) {
        return true;  // Continue on input error
    }
    
    // Handle input
    switch (event.type) {
        case INPUT_QUIT:
            return false;  // Exit the game
            
        case INPUT_CANCEL:
        case INPUT_STATS:
            return false;  // Exit the stats screen
            
        case INPUT_CONFIRM:
            // If the player has skill points, show the stat allocation menu
            if (player->skill_points > 0) {
                show_stat_allocation_menu(player);
                return true;  // Continue after allocation
            }
            return false;  // Exit the stats screen if no skill points
            
        default:
            // Any other key exits the stats screen if no skill points
            if (player->skill_points <= 0) {
                return false;
            }
            break;
    }
    
    return true;  // Continue by default
}

void run_stats_screen(character_t* player) {
    if (!player) {
        log_msg(ERROR, "stats_output", "Null player pointer");
        return;
    }
    
    // Render the initial stats screen
    render_stats_screen(player);
    
    // Handle input until the user exits
    while (handle_stats_input(player)) {
        // Re-render the stats screen after each input
        render_stats_screen(player);
    }
}

void shutdown_stats_output(void) {
    log_msg(INFO, "stats_output", "Stats output handler shut down");
}