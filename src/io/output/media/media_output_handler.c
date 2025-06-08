/**
 * @file media_output_handler.c
 * @brief Implements a handler for outputing media to the screen.
 */
#include "media_output_handler.h"

#include "../../../logging/logger.h"
#include "../../io_handler.h"        // Include this to access global nc and stdplane
#include "../common/output_handler.h"// For get_screen_dimensions and render_frame
#include "media_files.h"

#include <notcurses/notcurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


/* =========================================================================
 * CONSTANTS AND DEFINITIONS
 * ========================================================================= */

// Global variables
#define MAX_RESOURCES 128
static loaded_visual_t resources[MAX_RESOURCES];// Array to hold loaded visuals
static int resource_count = 0;

/* =========================================================================
 * FORWARD DECLARATIONS
 * ========================================================================= */

static void free_media_resource(loaded_visual_t* resource);

/* =========================================================================
* INITIALIZATION AND CLEANUP
* ========================================================================= */

bool init_media_output(void) {
    if (!gio->nc) {
        log_msg(ERROR, "media_output", "Null Notcurses instance provided");
        return false;
    }

    if (!gio->stdplane) {
        log_msg(ERROR, "media_output", "Null standard plane provided");
        return false;
    }

    // Initialize resources array
    memset(resources, 0, sizeof(resources));
    resource_count = 0;

    // Ensure the media directory exists
    if (!directory_exists(MEDIA_PATH)) {
        log_msg(WARNING, "media_output", "Media directory does not exist: %s", MEDIA_PATH);
    }

    return true;
}

// Clean up the media handler
void shutdown_media_output(void) {
    media_cleanup();
}

void media_cleanup(void) {
    // Free all loaded resources
    for (int i = 0; i < resource_count; i++) {
        if (resources[i].is_loaded) {
            free_media_resource(&resources[i]);
        }
    }
    resource_count = 0;
}

void destroy_media(loaded_visual_t* media) {
    free_media_resource(media);
}

/**\n * @brief Force a refresh of the media display\n * @return bool indicating success or failure\n */
bool refresh_media_display(void) {
    // Force a redraw of the terminal with error checking
    bool result = notcurses_render(gio->nc);

    if (!result) {
        log_msg(ERROR, "media_output", "Failed to refresh media display");
    }

    return result;
}

/* =========================================================================
 * LOAD AND READY FUNCTIONS
 * ========================================================================= */

// Load a media resource
loaded_visual_t* load_media(const char* filename) {
    // Validate parameters
    if (!filename) {
        log_msg(ERROR, "media_output", "Invalid filename for load_media");
        return NULL;
    }

    media_type_t media_type = get_file_type(filename);

    // Build the full file path
    char* filepath = build_filepath(filename, media_type);
    if (!filepath) {
        return NULL;
    }

    // Check if we already have this resource loaded
    for (int i = 0; i < resource_count; i++) {
        if (strcmp(resources[i].path, filepath) == 0) {
            free(filepath);
            return &resources[i];// Return existing resource
        }
    }

    // Make sure we have space for a new resource
    if (resource_count >= MAX_RESOURCES) {
        // Could implement a LRU cache here, but for simplicity, just fail
        free(filepath);
        return NULL;
    }

    // Load the visual from file with detailed error handling
    struct ncvisual* visual = ncvisual_from_file(filepath);
    if (!visual) {
        log_msg(ERROR, "media_output", "Failed to load media: %s (check if file exists)", filepath);
        return NULL;
    }

    // Allocate a new resource
    loaded_visual_t* resource = &resources[resource_count];
    if (!resource) {
        log_msg(ERROR, "media_output", "Failed to allocate memory for loaded visual");
        ncvisual_destroy(visual);
        return NULL;
    }
    resource_count++;

    // Initialize the structure
    memset(resource, 0, sizeof(loaded_visual_t));
    resource->visual = visual;
    resource->plane = NULL;// Will be created when displayed

    // Set resource properties
    resource->media_type = media_type;
    resource->is_loaded = true;
    resource->is_playing = false;
    resource->path = strdup(filepath);
    if (!resource->path) {
        log_msg(ERROR, "media_output", "Failed to store path");
        ncvisual_destroy(visual);
        resource_count--;
        free(filepath);
        return NULL;
    }

    // Get visual dimensions
    struct ncvisual_options vopts = {0};
    struct ncvgeom geom = {0};
    int geo_ret = ncvisual_geom(gio->nc, visual, &vopts, &geom);

    if (geo_ret) {
        log_msg(WARNING, "media_output", "Failed to get visual geometry, using default dimensions");
        // Use defaults if we can't get the dimensions
        resource->og_width = 20; // Default width
        resource->og_height = 20;// Default height
    } else {
        // Store dimensions with detailed logging
        if (geom.pixx <= 0 || geom.pixy <= 0) {
            log_msg(WARNING, "media_output", "Invalid media dimensions: %dx%d, using defaults",
                    geom.pixx, geom.pixy);
            resource->og_width = 20; // Default width
            resource->og_height = 20;// Default height
        } else {
            resource->og_width = geom.pixx; // Width in pixels
            resource->og_height = geom.pixy;// Height in pixels
        }
    }

    // set attributes based on media media_type
    switch (media_type) {
        case MEDIA_PNG:
            resource->frames = 1;// Single frame for static images
            break;
        case MEDIA_GIF:
            // Count frames by decoding the whole GIF
            resource->frames = 0;
            while (ncvisual_decode(visual) != 1) {
                resource->frames++;
            }
            // Reset to first frame
            ncvisual_destroy(visual);
            resource->visual = ncvisual_from_file(filepath);
            if (!resource->visual) {
                log_msg(ERROR, "media_output", "Failed to reload GIF after frame counting");
                free(resource->path);
                resource_count--;
                free(filepath);
                return NULL;
            }
            break;
        case MEDIA_MP4:
            // MP4 handling would go here
            log_msg(ERROR, "media_output", "MP4 loading not implemented yet");
            ncvisual_destroy(visual);
            resource_count--;
            free(resource->path);
            break;
        default:
            log_msg(ERROR, "media_output", "Unsupported media media_type");
            ncvisual_destroy(visual);
            free(filepath);
            return NULL;
    }

    free(filepath);
    return resource;
}

loaded_visual_t* ready_media(const char* filename, int x, int y, int height, int width, scale_type_t scale_type) {
    // Validate parameters
    if (!filename) {
        log_msg(ERROR, "media_output", "Null filename for display_media");
        return NULL;
    }

    // Allow height=0 for fullscreen and preserving aspect ratio
    if (height < 0) {
        log_msg(ERROR, "media_output", "Invalid height (%d) for display_media", height);
        return NULL;
    }

    // Load the media
    loaded_visual_t* resource = load_media(filename);
    if (!resource || !resource->is_loaded) {
        log_msg(ERROR, "media_output", "Failed to load media: %s", filename);
        return NULL;
    }

    // Set up scaling
    setup_scaling_options(resource, scale_type, width, height);

    // Set coordinates
    resource->options.y = y;
    resource->options.x = x;

    // Clean up existing plane if needed
    if (resource->plane) {
        ncplane_erase(resource->plane);// Clear contents first
        notcurses_render(gio->nc);     // Update display
        ncplane_destroy(resource->plane);
        resource->plane = NULL;
    }

    return resource;
}

/* =========================================================================
 * RESOURCE MANAGEMENT FUNCTIONS
 * ========================================================================= */

// Unload a specific media resource
bool unload_media(const char* filename) {
    if (!filename) {
        return false;
    }

    // Look for the resource in our cache
    for (int i = 0; i < resource_count; i++) {
        if (strcmp(resources[i].path, filename) == 0) {
            free_media_resource(&resources[i]);

            // Shift other resources if this isn't the last one
            if (i < resource_count - 1) {
                memmove(&resources[i], &resources[i + 1], (resource_count - i - 1) * sizeof(loaded_visual_t));
            }

            resource_count--;
            return true;
        }
    }

    return false;
}

/**
 * @brief Preload a media file into memory
 * @param filename File name to preload
 * @return true on success, false on failure
 */
bool preload_media(const char* filename) {
    if (!filename) {
        log_msg(ERROR, "media_output", "Null filename for preload_media");
        return false;
    }

    // Try to load the media - this will cache it in our resources array
    loaded_visual_t* resource = load_media(filename);
    if (!resource) {
        log_msg(ERROR, "media_output", "Failed to preload media: %s", filename);
        return false;
    }

    log_msg(DEBUG, "media_output", "Successfully preloaded media: %s", filename);
    return true;
}

/**
 * @brief Reload media after terminal resize
 * @return true on success, false on failure
 */
bool reload_media_after_resize(void) {
    if (!gio->nc || !gio->stdplane) {
        log_msg(ERROR, "media_output", "Notcurses not properly initialized for resize");
        return false;
    }

    bool all_successful = true;

    // Reload all currently loaded resources
    for (int i = 0; i < resource_count; i++) {
        if (resources[i].is_loaded) {
            // Destroy the current plane if it exists
            if (resources[i].plane) {
                ncplane_destroy(resources[i].plane);
                resources[i].plane = NULL;
            }

            // If this was a fullscreen media, recalculate dimensions
            if (resources[i].options.scaling == NCSCALE_STRETCH) {
                int screen_width, screen_height;
                if (get_screen_dimensions(&screen_width, &screen_height)) {
                    // Update options for new screen size
                    if (resources[i].options.leny == (unsigned int) screen_height &&
                        resources[i].options.lenx == (unsigned int) screen_width) {
                        // This was fullscreen, update to new dimensions
                        resources[i].options.leny = screen_height;
                        resources[i].options.lenx = screen_width;
                    }
                }
            }

            // The visual itself doesn't need reloading, just the plane
            // will be recreated on next render
        }
    }

    log_msg(DEBUG, "media_output", "Media resources prepared for terminal resize");
    return all_successful;
}

/* =========================================================================
 * REENDERING AND DISPLAY FUNCTIONS
 * ========================================================================= */

/**
 * @brief Display a loaded media file on its assigned plane
 * @param media Pointer to a loaded media instance
 * @return true on success, false on failure
 */
bool media_output_render(loaded_visual_t* media) {
    if (!media || !media->is_loaded || !media->visual) {
        log_msg(ERROR, "media_output", "Invalid media resource for rendering");
        return false;
    }

    if (!gio->nc || !gio->stdplane) {
        log_msg(ERROR, "media_output", "Notcurses not properly initialized");
        return false;
    }

    // Create a plane if we don't have one
    if (!media->plane) {
        // Set the plane to be a child of the standard plane
        media->options.n = gio->stdplane;

        // Create the plane using the visual
        media->plane = ncvisual_blit(gio->nc, media->visual, &media->options);
        if (!media->plane) {
            log_msg(ERROR, "media_output", "Failed to create plane for media rendering");
            return false;
        }
    } else {
        // Use existing plane, just blit the visual to it
        if (ncvisual_blit(gio->nc, media->visual, &media->options) == NULL) {
            log_msg(ERROR, "media_output", "Failed to blit visual to existing plane");
            return false;
        }
    }

    // Render the changes
    if (notcurses_render(gio->nc) < 0) {
        log_msg(ERROR, "media_output", "Failed to render media to screen");
        return false;
    }

    return true;
}

/**
 * @brief Display the next frame of an animation or video
 * @param media Pointer to a loaded media instance
 * @return true on success, false on failure or end of media
 */
bool media_output_render_next_frame(loaded_visual_t* media) {
    if (!media || !media->is_loaded || !media->visual) {
        log_msg(ERROR, "media_output", "Invalid media resource for next frame");
        return false;
    }

    // Only applicable for animated media types
    if (media->media_type != MEDIA_GIF && media->media_type != MEDIA_MP4) {
        // For static images, just render the current frame
        return media_output_render(media);
    }

    // Decode the next frame
    int decode_result = ncvisual_decode(media->visual);
    if (decode_result == 1) {
        // End of animation reached
        log_msg(DEBUG, "media_output", "End of animation reached");
        return false;
    } else if (decode_result < 0) {
        log_msg(ERROR, "media_output", "Error decoding next frame");
        return false;
    }

    // Render the new frame
    return media_output_render(media);
}

/**
 * @brief Check if the notcurses implementation supports image loading
 * @return true if supported, false otherwise
 */
bool media_output_can_display_images(void) {
    if (!gio->nc) {
        log_msg(WARNING, "media_output", "Notcurses not initialized, cannot check image support");
        return false;
    }

    // Check if we can create a visual from a simple test
    // We'll check if the terminal supports any image blitting
    bool supports_images = false;

    // Check various blitters that support images
    if ((notcurses_check_pixel_support(gio->nc) > 0) || (notcurses_canopen_images(gio->nc))) {
        supports_images = true;// Pixel graphics supported or Image loading supported
    }

    return supports_images;
}

/**
 * @brief Check if the notcurses implementation supports video loading
 * @return true if supported, false otherwise
 */
bool media_output_can_display_videos(void) {
    if (!gio->nc) {
        log_msg(WARNING, "media_output", "Notcurses not initialized, cannot check video support");
        return false;
    }

    // Video support typically requires the same capabilities as images
    // plus the ability to decode video formats
    bool supports_videos = false;

    // Check if we have image support first (prerequisite for video)
    if (media_output_can_display_images()) {
        // Check if notcurses was compiled with video support
        // This is a basic check - in practice, you might want to
        // try loading a simple video file to be sure
        if (notcurses_canopen_videos(gio->nc)) {
            supports_videos = true;
        }
    }

    return supports_videos;
}

/* =========================================================================
 * SCALING FUNCTIONS
 * ========================================================================= */

// Helper function to set up scaling options
void setup_scaling_options(loaded_visual_t* visual, scale_type_t scale_type,
                           int target_width, int target_height) {
    if (!visual) {
        log_msg(ERROR, "media_output", "Invalid visual parameter for setup_scaling_options");
        return;
    }

    // Clear options first
    memset(&visual->options, 0, sizeof(visual->options));

    // Apply scaling based on type
    switch (scale_type) {
        case SCALE_PRESERVE:
            // Scale preserving aspect ratio
            visual->options.scaling = NCSCALE_SCALE_HIRES;
            if (target_width > 0 && target_height > 0) {
                // Calculate aspect-preserving dimensions
                double aspect_ratio = (double) visual->og_width / visual->og_height;
                int calc_width = target_width;
                int calc_height = (int) (target_width / aspect_ratio);

                if (calc_height > target_height) {
                    calc_height = target_height;
                    calc_width = (int) (target_height * aspect_ratio);
                }

                visual->options.leny = calc_height;
                visual->options.lenx = calc_width;
            } else {
                // Use original dimensions if target dimensions are invalid
                visual->options.leny = visual->og_height;
                visual->options.lenx = visual->og_width;
            }
            break;

        case SCALE_STRETCH:
            // Stretch to exact dimensions
            if (target_width > 0 && target_height > 0) {
                visual->options.scaling = NCSCALE_STRETCH;
                visual->options.leny = target_height;
                visual->options.lenx = target_width;
            } else {
                // Fall back to original size if dimensions are invalid
                visual->options.scaling = NCSCALE_NONE;
                visual->options.leny = visual->og_height;
                visual->options.lenx = visual->og_width;
            }
            break;

        case SCALE_CELL:
            // Scale to fit in a single cell
            visual->options.scaling = NCSCALE_SCALE_HIRES;
            visual->options.leny = 1;
            visual->options.lenx = 1;
            break;

        case SCALE_FULLSCREEN:
            // Scale to fit the entire terminal
            visual->options.scaling = NCSCALE_STRETCH;
            // Get terminal dimensions
            int screen_width, screen_height;
            if (get_screen_dimensions(&screen_width, &screen_height)) {
                visual->options.leny = screen_height;
                visual->options.lenx = screen_width;
                log_msg(DEBUG, "media_output", "Fullscreen scaling to %dx%d", screen_width, screen_height);
            } else {
                // Fallback to original dimensions
                visual->options.leny = visual->og_height;
                visual->options.lenx = visual->og_width;
                log_msg(WARNING, "media_output", "Could not get screen dimensions, using original size %dx%d",
                        visual->og_width, visual->og_height);
            }
            break;

        case SCALE_NONE:
        default:
            // No scaling - use original dimensions
            visual->options.scaling = NCSCALE_NONE;
            visual->options.leny = visual->og_height;
            visual->options.lenx = visual->og_width;
            break;
    }

    // Set blitter based on scaling type for better quality
    switch (scale_type) {
        case SCALE_CELL:
            visual->options.blitter = NCBLIT_1x1;// ASCII for single cell
            break;
        case SCALE_FULLSCREEN:
        case SCALE_STRETCH:
            visual->options.blitter = NCBLIT_PIXEL;// Best quality for large images
            break;
        default:
            visual->options.blitter = NCBLIT_2x2;// Good balance for most cases
            break;
    }

    log_msg(DEBUG, "media_output", "Scaling setup: type=%d, target=%dx%d, result=%dx%d, scaling=%d, blitter=%d",
            scale_type, target_width, target_height,
            visual->options.lenx, visual->options.leny,
            visual->options.scaling, visual->options.blitter);
}

/* =========================================================================
 * FILE AND PATH HANDLING FUNCTIONS
 * ========================================================================= */

/**
 * @brief Check if a directory exists
 * @param path Path to the directory
 * @return true if directory exists, false otherwise
 */
bool directory_exists(const char* path) {
    if (!path) {
        return false;
    }

    struct stat stat_buf;

    // Use stat to check if the path exists and is a directory
    if (stat(path, &stat_buf) == 0) {
        return S_ISDIR(stat_buf.st_mode);
    }

    return false;
}

media_type_t get_file_type(const char* filename) {
    if (is_file_extension(filename, ".png")) {
        return MEDIA_PNG;
    } else if (is_file_extension(filename, ".gif")) {
        return MEDIA_GIF;
    } else if (is_file_extension(filename, ".mp4")) {
        return MEDIA_MP4;
    } else {
        log_msg(ERROR, "media_output", "Unsupported file extension for: %s", filename);
        return MEDIA_UNSUPPORTED;
    }
}

// Check if a filename has a specific extension
bool is_file_extension(const char* filename, const char* extension) {
    if (!filename || !extension) {
        return false;
    }

    size_t filename_len = strlen(filename);
    size_t ext_len = strlen(extension);

    if (filename_len < ext_len) {
        return false;
    }

    return strncasecmp(filename + filename_len - ext_len, extension, ext_len) == 0;
}

// Build a full file path
char* build_filepath(const char* filename, media_type_t media_type) {
    // Allocate memory for the path
    char* filepath = malloc(MAX_PATH_LEN);
    if (!filepath) {
        log_msg(ERROR, "media_output", "Failed to allocate memory for file path");
        return NULL;
    }

    // Build the path based on the media media_type
    const char* subdir = "";
    switch (media_type) {
        case MEDIA_PNG:
            subdir = "png/";
            break;
        case MEDIA_GIF:
            subdir = "gif/";
            break;
        case MEDIA_MP4:
            subdir = "mp4/";
            break;
        default:
            break;
    }

    // Construct the full path
    snprintf(filepath, MAX_PATH_LEN, "%s%s%s", MEDIA_PATH, subdir, filename);

    return filepath;
}

/* =========================================================================
 * INTERNAL UTILITY FUNCTIONS
 * ========================================================================= */

// Free a media resource
/**
 * @brief Free a media resource
 *
 * @param resource The resource to be freed.
 */
static void free_media_resource(loaded_visual_t* resource) {
    if (!resource || !resource->is_loaded) {
        return;
    }

    // Stop any animation first
    resource->is_playing = false;

    // Clean up the plane properly - first erase its contents
    if (resource->plane) {
        // Erase the plane contents with transparency
        ncplane_erase(resource->plane);

        // Make sure the erase is visible
        notcurses_render(gio->nc);// Use direct render for reliability

        // Then destroy the plane
        ncplane_destroy(resource->plane);
        resource->plane = NULL;
    }

    // Free the visual resource
    if (resource->visual) {
        ncvisual_destroy(resource->visual);
        resource->visual = NULL;
    }

    // Free path if allocated
    if (resource->path) {
        free(resource->path);
        resource->path = NULL;
    }

    // Mark as not loaded
    resource->is_loaded = false;

    // Resource is part of static array, no need to free the structure itself
}