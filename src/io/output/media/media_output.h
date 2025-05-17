#ifndef MEDIA_OUTPUT_H
#define MEDIA_OUTPUT_H

#include <stdbool.h>
#include <notcurses/notcurses.h>

/* =========================================================================
 * MEDIA NAMES
 * ========================================================================= */

#define GOBLIN_PNG "goblin.png"      
#define SKULL_GIF "skull.gif"
#define DUNGEON_INTRO_GIF "dungeon_intro.gif"

/* =========================================================================
 * CONSTANTS AND DEFINITIONS
 * ========================================================================= */

#define MEDIA_PATH "src/art/"// Path to media files
#define MAX_PATH_LEN 256 // Maximum length for file paths

// Media types enumeration
typedef enum {
    MEDIA_PNG,
    MEDIA_GIF,
    MEDIA_MP4
} media_type_t;

// structure to hold media resources
typedef struct {
    void* data;                // Pointer to the data (ncvisual, ncplane, etc.)
    media_type_t type;         // Type of media
    char filepath[MAX_PATH_LEN]; // File path
    bool is_loaded;            // Is the media loaded
    bool is_playing;           // For animated media (GIF/MP4)
} media_resource_t;

// Global media resource cache to avoid reloading
#define MAX_RESOURCES 128
static media_resource_t resources[MAX_RESOURCES];
static int resource_count = 0;

/* Scaline modes provided by Notcurses:
typedef enum {
  NCSCALE_NONE,
  NCSCALE_SCALE,
  NCSCALE_STRETCH,
  NCSCALE_SCALE_HIRES,
  NCSCALE_NONE_HIRES,
  NCSCALE_INFLATE,
} ncscale_e;
 */

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
 * UTILITY FUNCTIONS FOR RENDERING OPTIONS
 * ========================================================================= */

/**
 * Create ncvisual options structure with the provided parameters
 * 
 * @param target_plane Target plane (NULL to create a new plane)
 * @param scaling Scaling mode to use
 * @param y Y coordinate in terminal cells
 * @param x X coordinate in terminal cells
 * @param height Height in terminal cells (0 for automatic)
 * @param width Width in terminal cells (0 for automatic)
 * @param blitter Blitter to use (NCBLIT_DEFAULT for default)
 * @param flags Optional flags
 * @return Initialized ncvisual_options structure
 */
struct ncvisual_options create_visual_options(struct ncplane* target_plane, 
                                             ncscale_e scaling,
                                             int y, int x,
                                             int height, int width,
                                             ncblitter_e blitter,
                                             uint64_t flags);

/**
 * Create a dynamically allocated ncvisual options structure
 * The caller is responsible for freeing the returned pointer
 * 
 * @param target_plane Target plane (NULL to create a new plane)
 * @param scaling Scaling mode to use
 * @param y Y coordinate in terminal cells
 * @param x X coordinate in terminal cells
 * @param height Height in terminal cells (0 for automatic)
 * @param width Width in terminal cells (0 for automatic)
 * @param blitter Blitter to use (NCBLIT_DEFAULT for default)
 * @param flags Optional flags
 * @return Dynamically allocated ncvisual_options structure (must be freed by caller)
 */
struct ncvisual_options* create_visual_options_alloc(struct ncplane* target_plane, 
                                                    ncscale_e scaling,
                                                    int y, int x,
                                                    int height, int width,
                                                    ncblitter_e blitter,
                                                    uint64_t flags);

/* =========================================================================
 * PNG DISPLAY FUNCTIONS
 * ========================================================================= */

/**
 * Display a PNG file at specified coordinates with scaling
 * 
 * @param filename PNG file name (will be loaded from src/art/png/)
 * @param x X coordinate in terminal cells
 * @param y Y coordinate in terminal cells
 * @param height Height in terminal cells
 * @param scaling Scaling mode to use
 * @return true on success, false on failure
 */
bool display_png_at(const char* filename, int x, int y, int height, ncscale_e scaling);

/**
 * Fill the background with a PNG file scaled to terminal size
 * 
 * @param filename PNG file name (will be loaded from src/art/png/)
 * @return true on success, false on failure
 */
bool fill_background_with_png(const char* filename);

/**
 * Fill a single terminal cell with a PNG image
 * 
 * @param filename PNG file name (will be loaded from src/art/png/)
 * @param y Y coordinate in terminal cells
 * @param x X coordinate in terminal cells
 * @return true on success, false on failure
 */
bool fill_cell_with_png(const char* filename, int y, int x);

/* =========================================================================
 * GIF DISPLAY FUNCTIONS
 * ========================================================================= */

/**
 * Display a GIF file at specified coordinates with scaling
 * 
 * @param filename GIF file name (will be loaded from src/art/gif/)
 * @param x X coordinate in terminal cells
 * @param y Y coordinate in terminal cells
 * @param height Height in terminal cells
 * @param scaling Scaling mode to use
 * @param fps Frames per second (0 for default)
 * @return true on success, false on failure
 */
bool display_gif_at(const char* filename, int x, int y, int height, ncscale_e scaling, float fps);

/**
 * Fill the background with a GIF file scaled to terminal size
 * 
 * @param filename GIF file name (will be loaded from src/art/gif/)
 * @param fps Frames per second (0 for default)
 * @return true on success, false on failure
 */
bool fill_background_with_gif(const char* filename, float fps);

/* =========================================================================
 * MP4 DISPLAY FUNCTIONS
 * ========================================================================= */

/**
 * Display an MP4 file at specified coordinates with scaling
 * 
 * @param filename MP4 file name (will be loaded from src/art/mp4/)
 * @param x X coordinate in terminal cells
 * @param y Y coordinate in terminal cells
 * @param height Height in terminal cells
 * @param scaling Scaling mode to use
 * @param fps Frames per second (0 for default)
 * @return true on success, false on failure
 */
bool display_mp4_at(const char* filename, int x, int y, int height, ncscale_e scaling, float fps);

/**
 * Fill the background with an MP4 file scaled to terminal size
 * 
 * @param filename MP4 file name (will be loaded from src/art/mp4/)
 * @param fps Frames per second (0 for default)
 * @return true on success, false on failure
 */
bool fill_background_with_mp4(const char* filename, float fps);

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
 * Pause/resume an animation
 * 
 * @param filename File name of the animation to toggle
 * @return true on success, false on failure
 */
bool toggle_animation_pause(const char* filename);

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
 * Reload media after terminal resize
 * 
 * @return true on success, false on failure
 */
bool reload_media_after_resize(void);

#endif// MEDIA_OUTPUT_H
