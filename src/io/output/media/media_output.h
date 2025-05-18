#ifndef MEDIA_OUTPUT_H
#define MEDIA_OUTPUT_H

#include <stdbool.h>
#include <notcurses/notcurses.h>

/* =========================================================================
 * CONSTANTS AND DEFINITIONS
 * ========================================================================= */

 /**
 * @brief Structure to represent a loaded visual
 * 
 * This is an opaque structure that encapsulates a Notcurses visual.
 */
typedef struct loaded_visual_s loaded_visual_t;

// Scale types for display functions
typedef enum {
    SCALE_NONE,         // No scaling, use original size
    SCALE_PRESERVE,    // Scale preserving aspect ratio
    SCALE_STRETCH,     // Stretch to exact dimensions
    SCALE_CELL,         // Scale to fit in a single cell
    SCALE_FULLSCREEN   // Scale to fill the entire screen
} scale_type_t;

// Media types enumeration
typedef enum {
    MEDIA_PNG,
    MEDIA_GIF,
    MEDIA_MP4
} media_type_t;

/* =========================================================================
 * MEDIA PATHS AND NAMES
 * ========================================================================= */

#define MEDIA_PATH "src/art/"// Path to media files
#define MAX_PATH_LEN 256 // Maximum length for file paths

#define GOBLIN_PNG "goblin.png"      
#define SKULL_GIF "skull.gif"
#define DUNGEON_INTRO_GIF "dungeon_intro.gif"

/* =========================================================================
 * INITIALIZATION AND CLEANUP
 * ========================================================================= */

/**
 * @brief Initialize the media output handler
 * 
 * Must be called after the common output handler is initialized.
 * 
 * @return true on success, false on failure
 */
bool init_media_output();

/**
 * @brief Shutdown the media output handler
 * 
 * Cleans up resources used by the media output handler.
 */
void shutdown_media_output(void);

/* =========================================================================
 * IMAGE DISPLAY FUNCTIONS
 * ========================================================================= */

/**
 * Display an image file at specified coordinates with scaling
 * 
 * @param filename Image file name (PNG supported, loaded from src/art/png/)
 * @param x X coordinate in terminal cells
 * @param y Y coordinate in terminal cells
 * @param height Height in terminal cells
 * @param width Width in terminal cells (0 for auto)
 * @param scale_type Scaling mode to use
 * @return true on success, false on failure
 */
bool display_image_at(const char* filename, int x, int y, int height, int width, scale_type_t scale_type);

/**
 * Fill the background with an image file scaled to terminal size
 * 
 * @param filename Image file name (PNG supported, loaded from src/art/png/)
 * @return true on success, false on failure
 */
bool fill_background_with_image(const char* filename);

/**
 * Fill a single terminal cell with an image
 * 
 * @param filename Image file name (PNG supported, loaded from src/art/png/)
 * @param x X coordinate in terminal cells
 * @param y Y coordinate in terminal cells
 * @return true on success, false on failure
 */
bool fill_cell_with_image(const char* filename, int x, int y);

/* =========================================================================
 * ANIMATION DISPLAY FUNCTIONS
 * ========================================================================= */

/**
 * Display an animation file at specified coordinates with scaling
 * 
 * @param filename Animation file name (GIF supported, loaded from src/art/gif/)
 * @param x X coordinate in terminal cells
 * @param y Y coordinate in terminal cells
 * @param height Height in terminal cells
 * @param width Width in terminal cells (0 for auto)
 * @param scaling Scaling mode to use
 * @param fps Frames per second
 * @param loop Whether to loop the animation
 * @return true on success, false on failure
 */
bool display_animation_at(const char* filename, int x, int y, int height, int width, scale_type_t scale_type, float fps, bool loop);

/**
 * Fill the background with an animation file scaled to terminal size
 * 
 * @param filename Animation file name (GIF supported, loaded from src/art/gif/)
 * @param fps Frames per second
 * @param loop Whether to loop the animation
 * @return true on success, false on failure
 */
bool fill_background_with_animation(const char* filename, float fps, bool loop);

/* =========================================================================
 * ANIMATION CONTROL FUNCTIONS
 * ========================================================================= */

/**
 * Stop playing an animated media (GIF or MP4)
 * 
 * @param filename File name of the animation to stop
 * @return true on success, false on failure
 */
bool stop_animation(const char* filename);

/**
 * Stop all animations
 */
void stop_all_animations(void);

/**
 * Refresh media display
 * 
 * @return true on success, false on failure
 */
bool refresh_media_display(void);

/* =========================================================================
 * RESOURCE MANAGEMENT FUNCTIONS
 * ========================================================================= */

/**
 * Unload a specific media resource
 * 
 * @param filename File name to unload
 * @return true on success, false on failure
 */
bool unload_media(const char* filename);

/**
 * Preload a media file into memory
 * 
 * @param filename File name to preload
 * @return true on success, false on failure
 */
bool preload_media(const char* filename);

/**
 * Reload media after terminal resize
 * 
 * @return true on success, false on failure
 */
bool reload_media_after_resize(void);

#endif// MEDIA_OUTPUT_H
