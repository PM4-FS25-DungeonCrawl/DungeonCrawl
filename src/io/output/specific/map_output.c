#include "map_output.h"
#include "../colors.h"
#include "../common/common_output.h"
#include "../../input/input_handler.h"
#include "../../../logging/logger.h"
#include "../../../local/local.h"  // For localization
#include "../../../character/character.h"
#include "../../../map/map.h"

#include <stdbool.h>
#include <string.h>

// Definitions for map rendering
#define MAP_BORDER_COLOR COLOR_UI_FG
#define MAP_BACKGROUND_COLOR COLOR_BLACK
#define PLAYER_COLOR COLOR_PLAYER
#define WALL_COLOR COLOR_WALL
#define FLOOR_COLOR COLOR_FLOOR
#define DOOR_COLOR COLOR_DOOR
#define FOG_COLOR 0x111111  // Very dark gray for unexplored areas

// Map tile symbols
static const char* map_symbols[] = {
    " ",    // HIDDEN
    ".",    // FLOOR
    "#",    // WALL
    "+",    // START_DOOR
    "*",    // EXIT_DOOR
    "k",    // KEY
    "♥",    // LIFE_FOUNTAIN
    "♦",    // MANA_FOUNTAIN
    "G",    // GOBLIN
    "M",    // MONSTER
    "?",    // UNKNOWN
};

// Map tile colors (foreground)
static const uint32_t map_colors[] = {
    FOG_COLOR,              // HIDDEN
    FLOOR_COLOR,            // FLOOR
    WALL_COLOR,             // WALL
    DOOR_COLOR,             // START_DOOR
    DOOR_COLOR,             // EXIT_DOOR
    COLOR_YELLOW,           // KEY
    COLOR_RED,              // LIFE_FOUNTAIN
    COLOR_BLUE,             // MANA_FOUNTAIN
    COLOR_MONSTER,          // GOBLIN
    COLOR_MONSTER,          // MONSTER
    COLOR_MAGENTA,          // UNKNOWN
};

// Private state
static int current_light_radius = 3;
static char has_key = 0;  // Whether the player has the key

bool init_map_output(void) {
    log_msg(INFO, "map_output", "Map output handler initialized");
    return true;
}

bool render_map(const map_tile_t* map, const map_tile_t* revealed_map,
               int height, int width, vector2d_t map_anchor, 
               vector2d_t player_pos, const character_t* player) {
    if (!map || !revealed_map) {
        log_msg(ERROR, "map_output", "Null map or revealed map");
        return false;
    }
    
    // Calculate the map border area
    int border_x = map_anchor.dx - 1;
    int border_y = map_anchor.dy - 1;
    int border_width = width + 2;
    int border_height = height + 2;
    
    // Draw map border
    draw_box(border_y, border_x, border_height, border_width, "Map", MAP_BORDER_COLOR, MAP_BACKGROUND_COLOR);
    
    // Draw each map tile based on revealed state
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int tile_x = map_anchor.dx + x;
            int tile_y = map_anchor.dy + y;
            
            // Get the revealed state for this tile
            map_tile_t tile = HIDDEN;
            if (revealed_map[y * width + x] != HIDDEN) {
                tile = revealed_map[y * width + x];
            }
            
            // Determine the symbol and color for this tile
            const char* symbol = map_symbols[tile];
            uint32_t color = map_colors[tile];
            
            // Special case for player position
            if (x == player_pos.dx && y == player_pos.dy) {
                symbol = "@";
                color = PLAYER_COLOR;
            }
            
            // Draw the tile
            print_text(tile_y, tile_x, symbol, color, MAP_BACKGROUND_COLOR);
        }
    }
    
    // Draw player stats next to the map
    int stats_x = border_x + border_width + 2;
    int stats_y = border_y;
    
    // Draw player health and mana
    char health_str[32];
    char mana_str[32];
    snprintf(health_str, sizeof(health_str), "Health: %d/%d", 
             player->current_resources.health, player->max_resources.health);
    snprintf(mana_str, sizeof(mana_str), "Mana: %d/%d", 
             player->current_resources.mana, player->max_resources.mana);
    
    print_text(stats_y, stats_x, "Player Stats", COLOR_UI_HIGHLIGHT, COLOR_UI_BG);
    print_text(stats_y + 1, stats_x, health_str, COLOR_RED, COLOR_UI_BG);
    print_text(stats_y + 2, stats_x, mana_str, COLOR_BLUE, COLOR_UI_BG);
    
    // Draw key status
    const char* key_status = has_key ? "Key: Found" : "Key: Not Found";
    print_text(stats_y + 4, stats_x, key_status, COLOR_YELLOW, COLOR_UI_BG);
    
    // Draw level info
    char level_str[32];
    snprintf(level_str, sizeof(level_str), "Level: %d", player->level);
    print_text(stats_y + 6, stats_x, level_str, COLOR_UI_FG, COLOR_UI_BG);
    
    // Draw controls help at the bottom
    int controls_y = border_y + border_height + 1;
    int controls_x = border_x;
    
    print_text(controls_y, controls_x, "Controls: Arrows - Move | M - Menu | L - Stats | ESC - Pause", 
              COLOR_UI_FG, COLOR_UI_BG);
    
    // Render the frame
    render_frame();
    
    return true;
}

void calculate_visible_tiles(const map_tile_t* map, map_tile_t* revealed_map,
                          int height, int width, vector2d_t player_pos, int light_radius) {
    if (!map || !revealed_map) {
        log_msg(ERROR, "map_output", "Null map or revealed map");
        return;
    }
    
    // Store the light radius for future reference
    current_light_radius = light_radius;
    
    // Reveal all tiles within the light radius using a simple distance check
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Calculate the distance from the player
            int dx = abs(x - player_pos.dx);
            int dy = abs(y - player_pos.dy);
            int distance = (dx > dy) ? dx : dy;  // Chebyshev distance
            
            // If the tile is within the light radius, reveal it
            if (distance <= light_radius) {
                revealed_map[y * width + x] = map[y * width + x];
                
                // Check if this is a key, update the key status
                if (map[y * width + x] == KEY && 
                    x == player_pos.dx && y == player_pos.dy) {
                    has_key = 1;
                }
            }
        }
    }
}

// Helper for translating input to movement direction
static void get_direction_from_input(input_t input, int* dx, int* dy) {
    *dx = 0;
    *dy = 0;
    
    switch (input) {
        case INPUT_UP:
            *dy = -1;
            break;
        case INPUT_DOWN:
            *dy = 1;
            break;
        case INPUT_LEFT:
            *dx = -1;
            break;
        case INPUT_RIGHT:
            *dx = 1;
            break;
        default:
            break;
    }
}

// Map mode result enum (internal to this file, aligns with map_mode_result_t)
enum {
    MAP_CONTINUE = 0,
    MAP_QUIT,
    MAP_NEXT_FLOOR,
    MAP_COMBAT,
    MAP_SHOW_MENU,
    MAP_SHOW_STATS
};

int update_map_state(const input_event_t* event, character_t* player,
                   map_tile_t* map, map_tile_t* revealed_map,
                   int height, int width, vector2d_t* player_pos) {
    if (!event || !player || !map || !revealed_map || !player_pos) {
        log_msg(ERROR, "map_output", "Invalid parameters for update_map_state");
        return MAP_QUIT;  // Default to quit on error
    }
    
    // Check for special inputs first
    if (event->type == INPUT_QUIT) {
        return MAP_QUIT;
    } else if (event->type == INPUT_MENU) {
        return MAP_SHOW_MENU;
    } else if (event->type == INPUT_STATS) {
        return MAP_SHOW_STATS;
    } else if (event->type == INPUT_CANCEL) {
        return MAP_SHOW_MENU;  // ESC also shows menu
    }
    
    // Handle movement inputs
    int dx = 0, dy = 0;
    get_direction_from_input(event->type, &dx, &dy);
    
    if (dx != 0 || dy != 0) {
        // Calculate new position
        int new_x = player_pos->dx + dx;
        int new_y = player_pos->dy + dy;
        
        // Check if the new position is within the map bounds
        if (new_x >= 0 && new_x < width && new_y >= 0 && new_y < height) {
            // Check what's at the new position and handle accordingly
            map_tile_t tile = map[new_y * width + new_x];
            
            switch (tile) {
                case WALL:
                    // Can't move through walls
                    break;
                case START_DOOR:
                    // Can't go back through the start door
                    break;
                case KEY:
                    // Collect the key
                    player_pos->dx = new_x;
                    player_pos->dy = new_y;
                    has_key = 1;
                    map[new_y * width + new_x] = FLOOR;  // Replace key with floor
                    revealed_map[new_y * width + new_x] = FLOOR;
                    break;
                case EXIT_DOOR:
                    // Go to next floor if player has the key
                    if (has_key) {
                        player_pos->dx = new_x;
                        player_pos->dy = new_y;
                        has_key = 0;  // Reset key for next floor
                        return MAP_NEXT_FLOOR;
                    }
                    break;
                case LIFE_FOUNTAIN:
                    // Restore health
                    player_pos->dx = new_x;
                    player_pos->dy = new_y;
                    player->current_resources.health = player->max_resources.health;
                    map[new_y * width + new_x] = FLOOR;  // Use up the fountain
                    revealed_map[new_y * width + new_x] = FLOOR;
                    break;
                case MANA_FOUNTAIN:
                    // Restore mana
                    player_pos->dx = new_x;
                    player_pos->dy = new_y;
                    player->current_resources.mana = player->max_resources.mana;
                    map[new_y * width + new_x] = FLOOR;  // Use up the fountain
                    revealed_map[new_y * width + new_x] = FLOOR;
                    break;
                case GOBLIN:
                case MONSTER:
                    // Start combat
                    player_pos->dx = new_x;
                    player_pos->dy = new_y;
                    map[new_y * width + new_x] = FLOOR;  // Remove monster from map
                    revealed_map[new_y * width + new_x] = FLOOR;
                    return MAP_COMBAT;
                default:
                    // Move to the new position for other tiles
                    player_pos->dx = new_x;
                    player_pos->dy = new_y;
                    break;
            }
            
            // Update fog of war after movement
            calculate_visible_tiles(map, revealed_map, height, width, *player_pos, current_light_radius);
        }
    }
    
    // Default result is to continue
    return MAP_CONTINUE;
}

void shutdown_map_output(void) {
    log_msg(INFO, "map_output", "Map output handler shut down");
}