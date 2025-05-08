#include "inventory_output.h"
#include "../colors.h"
#include "../common/common_output.h"
#include "../../input/input_handler.h"
#include "../../menu/dialog.h"
#include "../../../logging/logger.h"
#include "../../../local/local.h"  // For localization
#include "../../../character/character.h"
#include "../../../item/gear.h"
#include "../../../item/potion.h"

#include <stdio.h>
#include <string.h>

// Define constants for inventory rendering
#define MAX_ITEMS_PER_PAGE 10
#define GEAR_SLOTS 6  // Head, Body, Hands, Legs, Feet, Weapon

// Equipment slot names
static const char* slot_names[] = {
    "Head",
    "Body",
    "Hands",
    "Legs",
    "Feet",
    "Weapon"
};

// Forward declarations of helper functions
static bool handle_inventory_input(character_t* player, int* selected_index, int item_count);
static bool use_selected_item(character_t* player, int selected_index);

bool init_inventory_output(void) {
    log_msg(INFO, "inventory_output", "Inventory output handler initialized");
    return true;
}

bool render_inventory(const character_t* player) {
    if (!player) {
        log_msg(ERROR, "inventory_output", "Null player pointer");
        return false;
    }
    
    // Get screen dimensions for layout
    int screen_width, screen_height;
    if (!get_screen_dimensions(&screen_width, &screen_height)) {
        log_msg(ERROR, "inventory_output", "Failed to get screen dimensions");
        return false;
    }
    
    // Clear the screen
    clear_screen(COLOR_UI_BG);
    
    // Draw the inventory window
    int window_width = 70;
    int window_height = 24;
    int window_x = (screen_width - window_width) / 2;
    int window_y = (screen_height - window_height) / 2;
    
    draw_box(window_y, window_x, window_height, window_width, "Inventory", COLOR_UI_FG, COLOR_UI_BG);
    
    // Draw equipment section
    int equip_x = window_x + 2;
    int equip_y = window_y + 2;
    
    print_text(equip_y, equip_x, "Equipment", COLOR_UI_HIGHLIGHT, COLOR_UI_BG);
    
    // Draw equipment slots
    // In a real implementation, we would access the player's equipment
    // For now, we'll create placeholder content
    for (int i = 0; i < GEAR_SLOTS; i++) {
        char slot_str[64];
        snprintf(slot_str, sizeof(slot_str), "%s: %s", slot_names[i], 
                 "[Empty]");  // Replace with actual equipment names
        
        print_text(equip_y + i + 1, equip_x + 2, slot_str, COLOR_UI_FG, COLOR_UI_BG);
    }
    
    // Draw inventory items section
    int items_x = window_x + window_width / 2;
    int items_y = window_y + 2;
    
    print_text(items_y, items_x, "Items", COLOR_UI_HIGHLIGHT, COLOR_UI_BG);
    
    // Draw inventory items
    // In a real implementation, we would access the player's inventory
    // For now, we'll create placeholder content
    int item_count = 0;  // Replace with actual inventory count
    if (item_count == 0) {
        print_text(items_y + 2, items_x + 2, "No items", COLOR_UI_FG, COLOR_UI_BG);
    } else {
        // Draw items
        for (int i = 0; i < item_count && i < MAX_ITEMS_PER_PAGE; i++) {
            char item_str[64];
            snprintf(item_str, sizeof(item_str), "%d. %s", i + 1, 
                     "Item name");  // Replace with actual item names
            
            print_text(items_y + i + 2, items_x + 2, item_str, COLOR_UI_FG, COLOR_UI_BG);
        }
    }
    
    // Draw player stats (brief summary)
    int stats_x = window_x + 2;
    int stats_y = window_y + GEAR_SLOTS + 4;
    
    print_text(stats_y, stats_x, "Character Stats", COLOR_UI_HIGHLIGHT, COLOR_UI_BG);
    
    char health_str[32];
    char mana_str[32];
    snprintf(health_str, sizeof(health_str), "Health: %d / %d", 
             player->current_resources.health, player->max_resources.health);
    snprintf(mana_str, sizeof(mana_str), "Mana: %d / %d", 
             player->current_resources.mana, player->max_resources.mana);
    
    print_text(stats_y + 1, stats_x + 2, health_str, COLOR_RED, COLOR_UI_BG);
    print_text(stats_y + 2, stats_x + 2, mana_str, COLOR_BLUE, COLOR_UI_BG);
    
    // Draw combat stats
    int combat_x = items_x;
    int combat_y = items_y + MAX_ITEMS_PER_PAGE + 3;
    
    print_text(combat_y, combat_x, "Combat Stats", COLOR_UI_HIGHLIGHT, COLOR_UI_BG);
    
    char atk_str[32];
    char def_str[32];
    snprintf(atk_str, sizeof(atk_str), "Attack: %d Physical, %d Magical", 
             calculate_physical_attack(&player->base_stats), 
             calculate_magical_attack(&player->base_stats));
    snprintf(def_str, sizeof(def_str), "Defense: %d Physical, %d Magical", 
             calculate_physical_defense(&player->base_stats), 
             calculate_magical_defense(&player->base_stats));
    
    print_text(combat_y + 1, combat_x + 2, atk_str, COLOR_UI_FG, COLOR_UI_BG);
    print_text(combat_y + 2, combat_x + 2, def_str, COLOR_UI_FG, COLOR_UI_BG);
    
    // Draw controls help
    print_text(window_y + window_height - 2, window_x + 4, 
              "Controls: Up/Down - Navigate | Enter - Use/Equip | ESC - Exit", 
              COLOR_UI_FG, COLOR_UI_BG);
    
    // Render the frame
    render_frame();
    
    return true;
}

bool display_gear_details(const gear_t* item, int x, int y) {
    if (!item) {
        log_msg(ERROR, "inventory_output", "Null item pointer");
        return false;
    }
    
    // Draw item info box
    int box_width = 40;
    int box_height = 10;
    
    draw_box(y, x, box_height, box_width, item->name, COLOR_UI_FG, COLOR_UI_BG);
    
    // Draw item details
    char slot_str[32];
    char rarity_str[32];
    char stats_str[32];
    
    // Convert slot type to string
    const char* slot_type = "Unknown";
    if (item->slot < GEAR_SLOTS) {
        slot_type = slot_names[item->slot];
    }
    
    // Convert rarity to string
    const char* rarity_name = "Common";
    uint32_t rarity_color = COLOR_WHITE;
    
    switch (item->rarity) {
        case RARITY_COMMON:
            rarity_name = "Common";
            rarity_color = COLOR_WHITE;
            break;
        case RARITY_UNCOMMON:
            rarity_name = "Uncommon";
            rarity_color = COLOR_GREEN;
            break;
        case RARITY_RARE:
            rarity_name = "Rare";
            rarity_color = COLOR_BLUE;
            break;
        case RARITY_EPIC:
            rarity_name = "Epic";
            rarity_color = COLOR_MAGENTA;
            break;
        case RARITY_LEGENDARY:
            rarity_name = "Legendary";
            rarity_color = COLOR_YELLOW;
            break;
        default:
            break;
    }
    
    snprintf(slot_str, sizeof(slot_str), "Slot: %s", slot_type);
    snprintf(rarity_str, sizeof(rarity_str), "Rarity: %s", rarity_name);
    
    print_text(y + 2, x + 2, slot_str, COLOR_UI_FG, COLOR_UI_BG);
    print_text(y + 3, x + 2, rarity_str, rarity_color, COLOR_UI_BG);
    
    // Draw stat modifiers
    print_text(y + 5, x + 2, "Stat Modifiers:", COLOR_UI_HIGHLIGHT, COLOR_UI_BG);
    
    char mods_str[120] = ""; // Buffer for stat modifiers
    
    if (item->stat_mod.strength > 0)
        snprintf(mods_str + strlen(mods_str), sizeof(mods_str) - strlen(mods_str), 
                "STR +%d ", item->stat_mod.strength);
    if (item->stat_mod.intelligence > 0)
        snprintf(mods_str + strlen(mods_str), sizeof(mods_str) - strlen(mods_str), 
                "INT +%d ", item->stat_mod.intelligence);
    if (item->stat_mod.dexterity > 0)
        snprintf(mods_str + strlen(mods_str), sizeof(mods_str) - strlen(mods_str), 
                "DEX +%d ", item->stat_mod.dexterity);
    if (item->stat_mod.constitution > 0)
        snprintf(mods_str + strlen(mods_str), sizeof(mods_str) - strlen(mods_str), 
                "CON +%d ", item->stat_mod.constitution);
    
    if (strlen(mods_str) == 0) {
        strcpy(mods_str, "None");
    }
    
    print_text(y + 6, x + 4, mods_str, COLOR_UI_FG, COLOR_UI_BG);
    
    // Render the frame
    render_frame();
    
    return true;
}

bool display_potion_details(const potion_t* potion, int x, int y) {
    if (!potion) {
        log_msg(ERROR, "inventory_output", "Null potion pointer");
        return false;
    }
    
    // Draw potion info box
    int box_width = 40;
    int box_height = 8;
    
    draw_box(y, x, box_height, box_width, potion->name, COLOR_UI_FG, COLOR_UI_BG);
    
    // Draw potion details
    char type_str[32];
    char strength_str[32];
    char desc_str[64];
    
    // Convert potion type to string
    const char* type_name = "Unknown";
    uint32_t type_color = COLOR_WHITE;
    
    switch (potion->type) {
        case POTION_HEALTH:
            type_name = "Health";
            type_color = COLOR_RED;
            break;
        case POTION_MANA:
            type_name = "Mana";
            type_color = COLOR_BLUE;
            break;
        default:
            break;
    }
    
    snprintf(type_str, sizeof(type_str), "Type: %s", type_name);
    snprintf(strength_str, sizeof(strength_str), "Strength: %d", potion->power);
    
    // Generate description based on type and strength
    if (potion->type == POTION_HEALTH) {
        snprintf(desc_str, sizeof(desc_str), "Restores %d health points", potion->power);
    } else if (potion->type == POTION_MANA) {
        snprintf(desc_str, sizeof(desc_str), "Restores %d mana points", potion->power);
    } else {
        snprintf(desc_str, sizeof(desc_str), "Unknown effect");
    }
    
    print_text(y + 2, x + 2, type_str, type_color, COLOR_UI_BG);
    print_text(y + 3, x + 2, strength_str, COLOR_UI_FG, COLOR_UI_BG);
    print_text(y + 5, x + 2, "Effect:", COLOR_UI_HIGHLIGHT, COLOR_UI_BG);
    print_text(y + 6, x + 4, desc_str, COLOR_UI_FG, COLOR_UI_BG);
    
    // Render the frame
    render_frame();
    
    return true;
}

static bool handle_inventory_input(character_t* player, int* selected_index, int item_count) {
    if (!player || !selected_index) {
        return false;
    }
    
    // Get input
    input_event_t event;
    if (!get_input_blocking(&event)) {
        return true;  // Continue on input error
    }
    
    // Handle input
    switch (event.type) {
        case INPUT_UP:
            // Move selection up
            if (item_count > 0) {
                *selected_index = (*selected_index > 0) ? (*selected_index - 1) : (item_count - 1);
            }
            return true;
            
        case INPUT_DOWN:
            // Move selection down
            if (item_count > 0) {
                *selected_index = (*selected_index + 1) % item_count;
            }
            return true;
            
        case INPUT_CONFIRM:
            // Use/equip the selected item
            if (item_count > 0 && *selected_index >= 0 && *selected_index < item_count) {
                return use_selected_item(player, *selected_index);
            }
            return true;
            
        case INPUT_CANCEL:
            // Exit inventory
            return false;
            
        case INPUT_QUIT:
            // Exit game
            return false;
            
        default:
            return true;
    }
}

static bool use_selected_item(character_t* player, int selected_index) {
    // In a real implementation, this would use the item at the selected index
    // For now, this is a placeholder
    
    // Ask for confirmation
    bool confirmed = show_confirmation("Use Item", "Do you want to use this item?");
    
    if (confirmed) {
        // Use the item (placeholder)
        show_message("Item Used", "You used the item.");
    }
    
    return true;  // Continue in inventory screen
}

bool run_inventory(character_t* player) {
    if (!player) {
        log_msg(ERROR, "inventory_output", "Null player pointer");
        return false;
    }
    
    // Get the number of items in inventory (placeholder)
    int item_count = 0;  // Replace with actual inventory count
    
    // Initialize selection
    int selected_index = 0;
    
    // Render the initial inventory screen
    render_inventory(player);
    
    // Main inventory loop
    while (handle_inventory_input(player, &selected_index, item_count)) {
        // Re-render the inventory after each input
        render_inventory(player);
        
        // Highlight the selected item if any
        if (item_count > 0 && selected_index >= 0 && selected_index < item_count) {
            int window_width, window_height;
            get_screen_dimensions(&window_width, &window_height);
            
            int window_x = (window_width - 70) / 2;
            int window_y = (window_height - 24) / 2;
            
            int items_x = window_x + 70 / 2;
            int items_y = window_y + 2;
            
            // Highlight the selected item
            char item_str[64];
            snprintf(item_str, sizeof(item_str), "%d. %s", selected_index + 1, 
                     "Item name");  // Replace with actual item name
            
            print_text(items_y + selected_index + 2, items_x + 2, item_str, 
                      COLOR_UI_HIGHLIGHT, COLOR_UI_BG);
            
            // Render the frame
            render_frame();
        }
    }
    
    return false;  // No item was used (replace with actual result)
}

void shutdown_inventory_output(void) {
    log_msg(INFO, "inventory_output", "Inventory output handler shut down");
}