#ifndef MEDIA_OUTPUT_HANDLER_H
#define MEDIA_OUTPUT_HANDLER_H

#include <stdbool.h>
#include <notcurses/notcurses.h>

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
    MEDIA_MP4,
    MEDIA_UNSUPPORTED
} media_type_t;

/**
 * Media blitter options (rendering method)
 */
typedef enum media_blitter {
    MEDIA_BLITTER_DEFAULT = NCBLIT_DEFAULT,  // Use terminal's best option
    MEDIA_BLITTER_ASCII = NCBLIT_1x1,        // Use ASCII only
    MEDIA_BLITTER_HALF = NCBLIT_2x1,         // Use half blocks (▀ ▄)
    MEDIA_BLITTER_QUAD = NCBLIT_2x2,         // Use quadrant blocks (▖ ▗ ▘ ▙)
    MEDIA_BLITTER_BRAILLE = NCBLIT_BRAILLE,  // Use braille (⠀⠁⠂⠃...)
    MEDIA_BLITTER_PIXEL = NCBLIT_PIXEL,      // Use pixel graphics if available
} media_blitter_t;

/* =========================================================================
 * CONSTANTS AND DEFINITIONS
 * ========================================================================= */

 /**
 * @brief Structure to represent a loaded visual
 * 
 * This is an opaque structure that encapsulates a Notcurses visual.
 */

typedef struct loaded_visual_s {
    struct ncvisual* visual;        // The Notcurses visual
    struct ncplane* plane;          // The plane for rendering the visual
    struct ncvisual_options options;// Display options
    media_type_t media_type;              // Type of media (PNG, GIF, MP4)
    int width;                      // Original width
    int height;                     // Original height
    int frames;                     // Number of frames (for animations)
    bool is_playing;                // Whether an animation is currently playing
    bool is_loaded;                 // Whether the visual is loaded
    char* path;                     // Path to the file (for reloading)
} loaded_visual_t;

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
bool init_media_output(void);

/**
 * @brief Shutdown the media output handler
 * 
 * Cleans up resources used by the media output handler.
 */
void shutdown_media_output(void);

void media_cleanup(void);

void destroy_media(loaded_visual_t* media);

bool refresh_media_display(void);

/* =========================================================================
 * LOAD AND READY FUNCTIONS
 * ========================================================================= */

loaded_visual_t* load_media(const char* filename);

loaded_visual_t* ready_media(const char* filename, int x, int y, int height, int width, scale_type_t scale_type);

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


/* =========================================================================
 * REENDERING AND DISPLAY FUNCTIONS
 * ========================================================================= */

/**
 * Display a loaded media file on its assigned plane
 * @param media Pointer to a loaded media instance
 * @return true on success, false on failure
 */
bool media_output_render(loaded_visual_t* media);

/**
 * Display the next frame of an animation or video
 * @param media Pointer to a loaded media instance
 * @return true on success, false on failure or end of media
 */
bool media_output_render_next_frame(loaded_visual_t* media);

/**
 * Check if the notcurses implementation supports image loading
 * @return true if supported, false otherwise
 */
bool media_output_can_display_images(void);

/**
 * Check if the notcurses implementation supports video loading
 * @return true if supported, false otherwise
 */
bool media_output_can_display_videos(void);

/* =========================================================================
 * SCALING FUNCTIONS
 * ========================================================================= */

void setup_scaling_options(loaded_visual_t* visual, scale_type_t scale_type, int target_width, int target_height);

/* =========================================================================
 * FILE AND PATH HANDLING FUNCTIONS
 * ========================================================================= */

bool directory_exists(const char* path);

media_type_t get_file_type(const char* filename);

bool is_file_extension(const char* filename, const char* extension);

char* build_filepath(const char* filename, media_type_t media_type);

#endif /* MEDIA_OUTPUT_HANDLER_H */