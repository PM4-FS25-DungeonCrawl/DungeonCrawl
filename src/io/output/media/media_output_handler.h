#ifndef MEDIA_OUTPUT_HANDLER_H
#define MEDIA_OUTPUT_HANDLER_H

#include <notcurses/notcurses.h>
#include <stdbool.h>

// Scale types for display functions
typedef enum {
    SCALE_NONE,     // No scaling, use original size
    SCALE_PRESERVE, // Scale preserving aspect ratio
    SCALE_STRETCH,  // Stretch to exact dimensions
    SCALE_CELL,     // Scale to fit in a single cell
    SCALE_FULLSCREEN// Scale to fill the entire screen
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
    MEDIA_BLITTER_DEFAULT = NCBLIT_DEFAULT,// Use terminal's best option
    MEDIA_BLITTER_ASCII = NCBLIT_1x1,      // Use ASCII only
    MEDIA_BLITTER_HALF = NCBLIT_2x1,       // Use half blocks (▀ ▄)
    MEDIA_BLITTER_QUAD = NCBLIT_2x2,       // Use quadrant blocks (▖ ▗ ▘ ▙)
    MEDIA_BLITTER_BRAILLE = NCBLIT_BRAILLE,// Use braille (⠀⠁⠂⠃...)
    MEDIA_BLITTER_PIXEL = NCBLIT_PIXEL,    // Use pixel graphics if available
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
    media_type_t media_type;        // Type of media (PNG, GIF, MP4)
    unsigned int og_width;          // Original width
    unsigned int og_height;         // Original height
    unsigned int frames;            // Number of frames (for animations)
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

/***
 * @brief Clean up all loaded media resources
 *
 * Frees all loaded media resources and resets the handler.
 */
void media_cleanup(void);

/**
 * @brief Frees the memory associated with a loaded media resource.
 *
 * @param media Pointer to the loaded media instance to destroy
 */
void destroy_media(loaded_visual_t* media);

/**
 * @brief Force a refresh of the media display
 *
 * @return bool indicating success or failure
 */
bool refresh_media_display(void);

/* =========================================================================
 * LOAD AND READY FUNCTIONS
 * ========================================================================= */

/**
 * @brief load a media resource from file
 * they can be different types (PNG, GIF, MP4)
 *
 * @param filename File name to load
 * @return Pointer to a loaded visual instance
 */
loaded_visual_t* load_media(const char* filename);

/**
 * @brief Prepares a media resource for display
 *
 * @param filename File name to load
 * @param x X coordinate in terminal cells
 * @param y Y coordinate in terminal cells
 * @param height Height in terminal cells
 * @param width Width in terminal cells (0 for auto)
 * @param scale_type Scaling type to apply
 * @return Pointer to a loaded visual instance
 */
loaded_visual_t* ready_media(const char* filename, int x, int y, int height, int width, scale_type_t scale_type);

/* =========================================================================
 * RESOURCE MANAGEMENT FUNCTIONS
 * ========================================================================= */

/**
 * @brief Unload a specific media resource
 * 
 * @param filename File name to unload
 * @return true on success, false on failure
 */
bool unload_media(const char* filename);

/**
 * @brief Preload a media file into memory
 * 
 * @param filename File name to preload
 * @return true on success, false on failure
 */
bool preload_media(const char* filename);

/**
 * @brief Reload media after terminal resize
 * 
 * @return true on success, false on failure
 */
bool reload_media_after_resize(void);


/* =========================================================================
 * REENDERING AND DISPLAY FUNCTIONS
 * ========================================================================= */

/**
 * @brief Display a loaded media file on its assigned plane
 * @param media Pointer to a loaded media instance
 * @return true on success, false on failure
 */
bool media_output_render(loaded_visual_t* media);

/**
 * @brief Display the next frame of an animation or video
 * @param media Pointer to a loaded media instance
 * @return true on success, false on failure or end of media
 */
bool media_output_render_next_frame(loaded_visual_t* media);

/**
 * @brief Check if the notcurses implementation supports image loading
 * @return true if supported, false otherwise
 */
bool media_output_can_display_images(void);

/**
 * @brief Check if the notcurses implementation supports video loading
 * @return true if supported, false otherwise
 */
bool media_output_can_display_videos(void);

/* =========================================================================
 * SCALING FUNCTIONS
 * ========================================================================= */

/**
 * @brief Enable different scaling options for the loaded visual
 *
 * @param visual Pointer to the loaded visual instance
 * @param scale_type Scaling type to apply
 * @param target_width Width to scale to
 * @param target_height Height to scale to
 */
void setup_scaling_options(loaded_visual_t* visual, scale_type_t scale_type, int target_width, int target_height);

/* =========================================================================
 * FILE AND PATH HANDLING FUNCTIONS
 * ========================================================================= */

/**
 * @brief checks if a directory exists
 * @param path Path to the directory
 * @return success or failure
 */
bool directory_exists(const char* path);

/**
 * @brief Get the media type based on the file extension
 *
 * @param filename File name to check
 * @return Media type (PNG, GIF, MP4, or UNSUPPORTED)
 */
media_type_t get_file_type(const char* filename);

/**
 * @brief Check if a filename has a specific extension
 *
 * @param filename File name to check
 * @param extension Extension to check for
 * @return true if the file has the specified extension, false otherwise
 */
bool is_file_extension(const char* filename, const char* extension);

/**
 * @brief Build a file path for the specified media type
 *
 * @param filename File name to build the path for
 * @param media_type Type of media (PNG, GIF, MP4)
 * @return Pointer to the constructed file path
 */
char* build_filepath(const char* filename, media_type_t media_type);

#endif /* MEDIA_OUTPUT_HANDLER_H */