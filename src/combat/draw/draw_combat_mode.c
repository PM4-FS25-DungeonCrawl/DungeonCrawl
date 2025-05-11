/**
 * @file draw_combat_mode.c
 * @brief This file contains functions for rendering the combat mode in the Dungeon Crawl game
 */
#include "draw_combat_mode.h"

#include "../../character/character.h"
#include "../../game.h"
#include "../../src/common.h"
#include "../combat_mode.h"

#include <limits.h>
#include <notcurses/notcurses.h>
#include <unistd.h>

// Internal functions
int draw_resource_bar(vector2d_t anchor, const character_t* c);
char* find_data(const char* filename);
static struct ncplane* current_sprite_plane = NULL;
/**
 * @brief this function draws the combat view
 * @param anchor the anchor point of the combat view, representing the top left corner
 * @param player the player character
 * @param enemy the enemy character
 * @param race the enemy race
 * @param red_enemy_sprite if true, the enemy sprite will be drawn in red
 * @return the new anchor point after drawing the combat view
 * @note this function clears and presents the combat view
 */
vector2d_t draw_combat_view(const vector2d_t anchor, const character_t* player, const character_t* enemy, const enemy_race_t race, const bool red_enemy_sprite) {
    if (current_sprite_plane != NULL) {
        ncplane_destroy(current_sprite_plane);
        current_sprite_plane = NULL;
    }
    clear_screen(stdplane);
    const char* sprite_filename = NULL;

    // Copy of the anchor
    vector2d_t vec = {anchor.dx, anchor.dy};

    // Draw title
    ncplane_printf_yx(stdplane, vec.dy, anchor.dx + 20, "Combat Mode");
    vec.dy += 2;

    vec.dy = draw_resource_bar(vec, player);
    vec.dy = draw_resource_bar(vec, enemy);
    vec.dy += 2;

    // Set correct enemy sprite path based on race
    switch (race) {
        case RACE_GOBLIN:
            sprite_filename = "goblin.png";
            break;
        // Add more cases for other races
        default:
            log_msg(WARNING, "Draw combat mode", "invalid race for rendering sprite");
            return vec;
    }

    // Use a resource finder function similar to the demo code
    char* image_path = find_data(sprite_filename);
    if (image_path == NULL) {
        log_msg(ERROR, "Draw combat mode", "Could not find sprite file");
        return vec;
    }

    // Create a visual to render
    struct ncvisual* visual = ncvisual_from_file(image_path);
    free(image_path);// Free the path after use

    if (visual == NULL) {
        log_msg(ERROR, "Draw combat mode", "Could not load visual from file");
        return vec;
    }

    // Calculate dimensions and scaling
    unsigned screen_width, screen_height;
    ncplane_dim_yx(stdplane, &screen_height, &screen_width);

    // Target size for sprite
    int target_width = 30;
    int target_height = 30;

    // Set up visual options
    struct ncvisual_options vopts = {
            .n = stdplane,                      // Render to standard plane
            .y = vec.dy,                        // Y position
            .x = anchor.dx,                     // X position
            .begy = 0,                          // Start at beginning of visual
            .begx = 0,                          // Start at beginning of visual
            .leny = 0,                          // Use full height (0 means full)
            .lenx = 0,                          // Use full width (0 means full)
            .scaling = NCSCALE_SCALE,           // Scale to fit target dimensions
            .flags = NCVISUAL_OPTION_CHILDPLANE,// Create child plane
            .blitter = NCBLIT_2x1               // Let ncvisual pick appropriate blitter
    };

    // Get the geometry that will be used for this visual
    ncvgeom geom;
    if (ncvisual_geom(nc, visual, &vopts, &geom) != 0) {
        log_msg(ERROR, "Draw combat mode", "Failed to calculate visual geometry");
        ncvisual_destroy(visual);
        return vec;
    }

    // If terminal doesn't support pixel graphics, use an appropriate blitter
    if (!notcurses_check_pixel_support(nc)) {
        log_msg(INFO, "Draw combat mode", "Pixel blitting not supported, using half blocks");
        vopts.blitter = NCBLIT_2x1;// Using half blocks (▀ and ▄)
    } else {
        vopts.blitter = NCBLIT_PIXEL;
    }

    // Blit the visual to create a new plane
    struct ncplane* sprite_plane = ncvisual_blit(nc, visual, &vopts);
    current_sprite_plane = sprite_plane;


    if (sprite_plane == NULL) {
        log_msg(ERROR, "Draw combat mode", "Failed to blit visual");
        ncvisual_destroy(visual);
        return vec;
    }

    // Apply red tint if needed
    if (red_enemy_sprite) {
        nccell base = NCCELL_TRIVIAL_INITIALIZER;
        ncchannels_set_fg_rgb8(&base.channels, 255, 0, 0);           // Set foreground to red
        ncchannels_set_bg_alpha(&base.channels, NCALPHA_TRANSPARENT);// Transparent background
        ncplane_set_base_cell(sprite_plane, &base);
        nccell_release(sprite_plane, &base);// Release resources if necessary
    }

    // Update position for next element
    unsigned sprite_height, sprite_width;
    ncplane_dim_yx(sprite_plane, &sprite_height, &sprite_width);
    vec.dy += sprite_height;
    vec.dy += 1;

    // Render the result
    notcurses_render(nc);

    // Clean up resources
    ncvisual_destroy(visual);
    // Note: Don't destroy sprite_plane as it's now attached to stdplane

    return vec;
}
// clears the combat view
void clear_combat_view() {
    if (current_sprite_plane != NULL) {
        ncplane_destroy(current_sprite_plane);
        current_sprite_plane = NULL;
        notcurses_render(nc);// Re-render to show the changes
    }
}
/**
 * @brief Find a resource file (similar to the demo code)
 * @param filename The name of the file to find
 * @return A dynamically allocated string with the full path, or NULL if not found
 * @note The caller is responsible for freeing the returned string
 */
char* find_data(const char* filename) {
    // Try several common locations for the resource files
    const char* potential_paths[] = {
            "./resources/sprites/",
            "../resources/sprites/",
            "../../resources/sprites/",
            "./sprites/",
            "./",
            "../",
            // Add more potential paths as needed
    };

    const int num_paths = sizeof(potential_paths) / sizeof(potential_paths[0]);
    char temp_path[PATH_MAX];

    for (int i = 0; i < num_paths; i++) {
        snprintf(temp_path, PATH_MAX, "%s%s", potential_paths[i], filename);

        // Check if file exists and is readable
        if (access(temp_path, R_OK) == 0) {
            return strdup(temp_path);
        }
    }

    return NULL;
}

/**
 * @brief this function draws the combat menu
 * @param anchor the anchor point of the combat menu, representing the top left corner
 * @param menu_name the name of the menu
 * @param menu_options the options of the menu
 * @param menu_option_count the number of options in the menu
 * @param selected_index the index of the selected option
 * @param tail_msg the message to be displayed at the bottom of the menu. If the message is NULL, it will not be displayed.
 * @note this function only presents the drawn menu (without clearing the screen)
 */
void draw_combat_menu(const vector2d_t anchor, const char* menu_name, string_max_t* menu_options, const int menu_option_count, const int selected_index, const char* tail_msg) {
    if (menu_name == NULL || menu_options == NULL) {
        log_msg(ERROR, "Draw Combat Menu", "Menu options are NULL");
        return;
    }
    vector2d_t vec = {anchor.dx, anchor.dy};

    ncplane_set_channels(stdplane, DEFAULT_COLORS);
    ncplane_set_styles(stdplane, NCSTYLE_NONE);
    ncplane_putstr_yx(stdplane, vec.dy, 1, menu_name);
    vec.dy++;

    for (int i = 0; i < menu_option_count; i++) {
        char buffer[MAX_STRING_LENGTH];

        if (i == selected_index) {
            snprintf(buffer, sizeof(buffer), "> %-253s", (char*) &menu_options[i]);
            // Bold is handled through styling channels in notcurses
            ncplane_set_styles(stdplane, NCSTYLE_BOLD);
        } else {
            snprintf(buffer, sizeof(buffer), "  %-253s", (char*) &menu_options[i]);
            ncplane_set_styles(stdplane, NCSTYLE_NONE);
        }

        ncplane_putstr_yx(stdplane, vec.dy, anchor.dx, buffer);
        vec.dy++;
    }

    if (tail_msg != NULL) {
        ncplane_set_styles(stdplane, NCSTYLE_NONE);
        ncplane_printf_yx(stdplane, vec.dy + 2, 1, "%s", tail_msg);
    }
    notcurses_render(nc);
}

/**
 * @brief Draws the combat log
 * @param anchor The anchor point of the combat log, representing the top-left corner
 * @param combat_log_message The message to be displayed in the combat log
 * @note This function displays a combat log message and waits for user input before continuing
 */
void draw_combat_log(vector2d_t anchor, const char* combat_log_message) {
    if (combat_log_message == NULL) {
        log_msg(ERROR, "Draw Combat Mode", "given combat log messsage is NULL");
        return;
    }

    char message[MAX_STRING_LENGTH];
    snprintf(message, sizeof(message), "%s", combat_log_message);

    ncplane_set_channels(stdplane, DEFAULT_COLORS);

    ncplane_putstr_yx(stdplane, anchor.dy, anchor.dx, message);
    anchor.dy++;
    ncplane_putstr_yx(stdplane, anchor.dy, anchor.dx, "Press any key to continue...");
    anchor.dy++;
    notcurses_render(nc);

    // Waiting for input
    ncinput input;
    notcurses_get_blocking(nc, &input);
}

/**
 * @brief Draws the resource bar for a character
 * @param anchor The anchor point for the resource bar, representing the top-left corner
 * @param c A pointer to the character whose resources are to be displayed
 * @return The updated y-coordinate after drawing the resource bar
 * @note This function displays the character's name, health, mana, and stamina in a formatted bar
 */
int draw_resource_bar(vector2d_t anchor, const character_t* c) {
    char c_info[MAX_STRING_LENGTH];
    snprintf(c_info, sizeof(c_info), "%-20s | HP: %4d/%-4d | Mana: %4d/%-4d | Stamina: %4d/%-4d",
             c->name,
             c->current_resources.health, c->max_resources.health,
             c->current_resources.mana, c->max_resources.mana,
             c->current_resources.stamina, c->max_resources.stamina);

    // White on black
    ncplane_set_channels(stdplane, DEFAULT_COLORS);

    ncplane_putstr_yx(stdplane, anchor.dy, anchor.dx, c_info);
    anchor.dy++;
    return anchor.dy;
}

void draw_game_over() {
    clear_screen(stdplane);

    ncplane_set_channels(stdplane, RED_TEXT_COLORS);
    ncplane_printf_yx(stdplane, 1, 1, "Game over");
    ncplane_set_channels(stdplane, DEFAULT_COLORS);
    ncplane_printf_yx(stdplane, 2, 1, "Press any key to exit...");
    notcurses_render(nc);

    // Waiting for input
    ncinput input;
    notcurses_get_blocking(nc, &input);
}
