#include "media_output_handler.h"

#include "../../../logging/logger.h"
#include "../../io_handler.h"        // Include this to access global nc and stdplane
#include "../common/output_handler.h"// For get_screen_dimensions and render_frame
#include "media_files.h" 

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <notcurses/notcurses.h>



/* =========================================================================
 * CONSTANTS AND DEFINITIONS
 * ========================================================================= */

// Global variables
#define MAX_RESOURCES 128
static loaded_visual_t resources[MAX_RESOURCES]; // Array to hold loaded visuals
static int resource_count = 0;

/* =========================================================================
 * FORWARD DECLARATIONS
 * ========================================================================= */

static void free_media_resource(loaded_visual_t* resource);

 /* =========================================================================
 * INITIALIZATION AND CLEANUP
 * ========================================================================= */

 bool init_media_output(void) {
    log_msg(INFO, "media_output_handler", "Starting initialization");

    if (!nc) {
        log_msg(ERROR, "media_output", "Null Notcurses instance provided");
        return false;
    }

    if (!stdplane) {
        log_msg(ERROR, "media_output", "Null standard plane provided");
        return false;
    }

    // Initialize resources array
    memset(resources, 0, sizeof(resources));
    resource_count = 0;

    // Ensure the media directory exists
    if (!directory_exists(MEDIA_PATH)) {
        log_msg(ERROR, "media_output", "Failed to ensure media directory exists");
    }

    log_msg(INFO, "media_output", "Media output handler initialized");
    return true;
 }

 // Clean up the media handler
void shutdown_media_output(void) {
    media_cleanup();

    log_msg(INFO, "media_output", "Media output handler shut down");
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

// Refresh media display
bool refresh_media_display(void) {
    //TODO: fix
    log_msg(INFO, "media_output", "Forcing a complete redraw of the terminal");
    // Force a redraw of the terminal with error checking
    bool result = notcurses_render(nc);
    
    if (!result) {
        log_msg(ERROR, "media_output", "Failed to refresh media display");
    } else {
        log_msg(INFO, "media_output", "Successfully refreshed media display");
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
            return &resources[i]; // Return existing resource
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
    int geo_ret = ncvisual_geom(nc, visual, &vopts, &geom);

    if (geo_ret) {
        log_msg(WARNING, "media_output", "Failed to get visual geometry, using default dimensions");
        // Use defaults if we can't get the dimensions
        resource->width = 20; // Default width
        resource->height = 20;// Default height
    } else {
        // Store dimensions with detailed logging
        if (geom.pixx <= 0 || geom.pixy <= 0) {
            log_msg(WARNING, "media_output", "Invalid media dimensions: %dx%d, using defaults",
                    geom.pixx, geom.pixy);
            resource->width = 20; // Default width
            resource->height = 20;// Default height
        } else {
            resource->width = geom.pixx; // Width in pixels
            resource->height = geom.pixy;// Height in pixels
            log_msg(INFO, "media_output", "Got media dimensions: %dx%d", resource->width, resource->height);
        }
    }

    // set attributes based on media media_type
    switch (media_type) {
        case MEDIA_PNG:
            resource->frames = 1; // Single frame for static images
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
    log_msg(INFO, "media_output", "Successfully loaded media: %s", resource->path);
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
    
    log_msg(INFO, "media_output", "Setting up media for display: %s at position (%d,%d) size %dx%d", 
            filename, x, y, width, height);
    
    // Set up scaling
    setup_scaling_options(resource, scale_type, width, height);

    // Set coordinates
    resource->options.y = y;
    resource->options.x = x;

    // Clean up existing plane if needed
    if (resource->plane) {
        log_msg(INFO, "media_output", "Destroying existing plane before new display");
        ncplane_erase(resource->plane);  // Clear contents first
        notcurses_render(nc);            // Update display
        ncplane_destroy(resource->plane);
        resource->plane = NULL;
    }

    log_msg(INFO, "media_output", "Media successfully readied for display at position (%d,%d)", x, y);
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

// Preload a media file into memory
bool preload_media(const char* filename) {
    // coming soon
    return false;
}

// Reload media after terminal resize
bool reload_media_after_resize(void) {
    // coming soon
    return false;
}

/* =========================================================================
 * REENDERING AND DISPLAY FUNCTIONS
 * ========================================================================= */

bool media_output_render(loaded_visual_t* media) {
    //coming soon...
    return false;
}

bool media_output_render_next_frame(loaded_visual_t* media) {
    //coming soon...
    return false;
}

bool media_output_can_display_images(void) {
    //coming soon...
    return false;
}

bool media_output_can_display_videos(void) {
    //coming soon...
    return false;
}

/* =========================================================================
 * SCALING FUNCTIONS
 * ========================================================================= */

 // Helper function to set up scaling options
void setup_scaling_options(loaded_visual_t* visual, scale_type_t scale_type,
                                  int target_width, int target_height) {
    // Log scaling information
    log_msg(INFO, "media_output", "Setting up scaling: type=%d, target=%dx%d, source=%dx%d",
            scale_type, target_width, target_height, visual->width, visual->height);
            
    // Clear options first
    memset(&visual->options, 0, sizeof(visual->options));
    
    // Apply scaling based on type
    switch (scale_type) {
        case SCALE_PRESERVE:
            // Scale preserving aspect ratio
            visual->options.scaling = NCSCALE_SCALE;
            // When using NCSCALE_SCALE, a zero value for lenx/leny means auto-scale
            visual->options.leny = target_height > 0 ? target_height : 0;
            visual->options.lenx = target_width > 0 ? target_width : 0;
            log_msg(INFO, "media_output", "Using preserve aspect ratio scaling: %dx%d",
                    visual->options.lenx, visual->options.leny);
            break;

        case SCALE_STRETCH:
            // Stretch to exact dimensions
            if (target_width > 0 && target_height > 0) {
                visual->options.scaling = NCSCALE_STRETCH;
                visual->options.leny = target_height;
                visual->options.lenx = target_width;
                log_msg(INFO, "media_output", "Using stretch scaling: %dx%d",
                        visual->options.lenx, visual->options.leny);
            } else {
                // Fall back to no scaling if dimensions are invalid
                visual->options.scaling = NCSCALE_NONE;
                log_msg(INFO, "media_output", "Invalid stretch dimensions, using no scaling");
            }
            break;

        case SCALE_CELL:
            // Scale to fit in a single cell
            visual->options.scaling = NCSCALE_SCALE;
            visual->options.leny = 1;
            visual->options.lenx = 1;
            log_msg(INFO, "media_output", "Using cell scaling (1x1)");
            break;

        case SCALE_FULLSCREEN:
            // Scale to fit the entire terminal
            visual->options.scaling = NCSCALE_STRETCH;
            // Get terminal dimensions
            int screen_width, screen_height;
            if (get_screen_dimensions(&screen_width, &screen_height)) {
                visual->options.leny = screen_height;
                visual->options.lenx = screen_width;
                log_msg(INFO, "media_output", "Using fullscreen scaling: %dx%d",
                        visual->options.lenx, visual->options.leny);
            } else {
                // Fallback to original dimensions
                visual->options.leny = visual->height;
                visual->options.lenx = visual->width;
                log_msg(WARNING, "media_output", "Could not get screen dimensions, using original size");
            }
            break;

        case SCALE_NONE:
        default:
            // No scaling
            visual->options.scaling = NCSCALE_NONE;
            log_msg(INFO, "media_output", "Using no scaling");
            break;
    }
    
    // Set blitter to a reliable one
    visual->options.blitter = NCBLIT_2x1;
}

/* =========================================================================
 * FILE AND PATH HANDLING FUNCTIONS
 * ========================================================================= */

bool directory_exists(const char* path) {
    // coming soon
    return false;
}

media_type_t get_file_type(const char* filename) {
    if (is_file_extension(filename, ".png")) {
        log_msg(INFO, "media_output", "Detected PNG file: %s", filename);
        return MEDIA_PNG;
    } else if (is_file_extension(filename, ".gif")) {
        log_msg(INFO, "media_output", "Detected GIF file: %s", filename);
        return MEDIA_GIF;
    } else if (is_file_extension(filename, ".mp4")) {
        log_msg(INFO, "media_output", "Detected MP4 file: %s (not supported yet)", filename);
        return MEDIA_MP4;
    } else {
        log_msg(ERROR, "media_output", "Unsupported file extension for: %s", filename);
        return;
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
    }
    
    // Construct the full path
    snprintf(filepath, MAX_PATH_LEN, "%s%s%s", MEDIA_PATH, subdir, filename);
    log_msg(INFO, "media_output", "Built file path: %s", filepath);
    
    return filepath;
}

/* =========================================================================
 * INTERNAL UTILITY FUNCTIONS
 * ========================================================================= */

// Free a media resource
static void free_media_resource(loaded_visual_t* resource) {
    if (!resource || !resource->is_loaded) {
        return;
    }

    log_msg(INFO, "media_output", "Starting cleanup of resource: %s", 
            resource->path ? resource->path : "unknown");
    
    // Stop any animation first
    resource->is_playing = false;
    
    // Clean up the plane properly - first erase its contents
    if (resource->plane) {
        log_msg(INFO, "media_output", "Erasing plane contents before destruction");

        // Erase the plane contents with transparency
        ncplane_erase(resource->plane);

        // Make sure the erase is visible
        notcurses_render(nc);  // Use direct render for reliability

        // Then destroy the plane
        log_msg(INFO, "media_output", "Destroying plane");
        ncplane_destroy(resource->plane);
        resource->plane = NULL;
    }

    // Free the visual resource
    if (resource->visual) {
        log_msg(INFO, "media_output", "Destroying visual");
        ncvisual_destroy(resource->visual);
        resource->visual = NULL;
    }

    // Free path if allocated
    if (resource->path) {
        log_msg(INFO, "media_output", "Freeing resource path");
        free(resource->path);
        resource->path = NULL;
    }

    // Mark as not loaded
    resource->is_loaded = false;
    
    // Resource is part of static array, no need to free the structure itself
    log_msg(INFO, "media_output", "Visual resources cleanup complete");
}