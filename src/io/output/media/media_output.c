#include "media_output.h"

#include "../../../logging/logger.h"
#include "../../io_handler.h"        // Include this to access global nc and stdplane
#include "../common/output_handler.h"// For get_screen_dimensions and render_frame

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <notcurses/notcurses.h>

/* =========================================================================
 * CONSTANTS AND DEFINITIONS
 * ========================================================================= */

 // Opaque structure for loaded visuals
struct loaded_visual_s {
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
};

// Global variables
#define MAX_RESOURCES 128
static loaded_visual_t resources[MAX_RESOURCES]; // Array to hold loaded visuals
static int resource_count = 0;

/* =========================================================================
 * FORWARD DECLARATIONS
 * ========================================================================= */

// Forward declarations for internal functions
static bool get_cell_dimensions(int* cell_width, int* cell_height);
static void setup_scaling_options(loaded_visual_t* visual, scale_type_t scale_type, int target_width, int target_height);
static bool is_file_extension(const char* filename, const char* extension);
static loaded_visual_t* load_media(const char* filename, media_type_t media_type);
static bool ensure_directory_exists(const char* path);
static char* build_filepath(const char* filename, media_type_t media_type);
static void free_media_resource(loaded_visual_t* resource);
static void media_cleanup(void);
static bool display_image(loaded_visual_t* resource);
static bool display_animation(loaded_visual_t* resource, float fps, bool loop);
static loaded_visual_t* ready_media(const char* filename, int x, int y, int height, int width, scale_type_t scale_type);
static int animation_callback(struct ncvisual* ncv, struct ncvisual_options* vopts, const struct timespec* abstime, void* data);

/* =========================================================================
 * INITIALIZATION AND CLEANUP
 * ========================================================================= */

bool init_media_output() {
    log_msg(INFO, "media_output", "Starting initialization");

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
    if (!ensure_directory_exists(MEDIA_PATH)) {
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

/* =========================================================================
 * PNG DISPLAY FUNCTIONS
 * ========================================================================= */

// Display a PNG file at specified coordinates with scaling
bool display_image_at(const char* filename, int x, int y, int height, int width, scale_type_t scale_type) {
    // Validate parameters
    if (!filename || height < 0) { // Allow height=0 for automatic scaling
        log_msg(ERROR, "media_output", "Invalid parameters for display_png_at");
        return false;
    }
    loaded_visual_t* resource = ready_media(filename, x, y, height, width, scale_type);
    if (!resource) {
        log_msg(ERROR, "media_output", "Failed to load image for display");
        return false;
    }
    return display_image(resource);
}

// Fill the background with a PNG file scaled to terminal size
bool fill_background_with_image(const char* filename) {
    // Validate parameters
    if (!filename) {
        log_msg(ERROR, "media_output", "Invalid filename for fill_background_with_png");
        return false;
    }
    loaded_visual_t* resource = ready_media(filename, 0, 0, 0, 0, SCALE_FULLSCREEN);
    if (!resource) {
        log_msg(ERROR, "media_output", "Failed to load image for background");
        return false;
    }
    return display_image(resource);
}

// Fill a single terminal cell with a PNG image
bool fill_cell_with_image(const char* filename, int x, int y) {
    // Validate parameters
    if (!filename) {
        log_msg(ERROR, "media_output", "Invalid filename for fill_cell_with_png");
        return false;
    }
    loaded_visual_t* resource = ready_media(filename, x, y, 1, 1, SCALE_CELL);
    if (!resource) {
        log_msg(ERROR, "media_output", "Failed to load image for cell");
        return false;
    }
    return display_image(resource);
}

// Helper function for displaying images
static bool display_image(loaded_visual_t* resource) {
    // Validate parameters
    if (!resource || !resource->visual) {
        log_msg(ERROR, "media_output", "Invalid parameters for display_image");
        return false;
    }

    if (resource->media_type == MEDIA_GIF) {
        // If it's a GIF, display it as an animation with default 10 FPS
        return display_animation(resource, 10.0f, false);
    }
    
    // Clean up existing plane if needed
    if (resource->plane) {
        log_msg(INFO, "media_output", "Destroying existing plane");
        ncplane_destroy(resource->plane);
        resource->plane = NULL;
    }

    // Create a direct plane for the image
    struct ncplane_options opts = {0};
    opts.y = resource->options.y;
    opts.x = resource->options.x;
    opts.rows = resource->options.leny > 0 ? resource->options.leny : resource->height;
    opts.cols = resource->options.lenx > 0 ? resource->options.lenx : resource->width;

    log_msg(INFO, "media_output", "Creating plane (%d,%d) with size %dx%d", 
            opts.x, opts.y, opts.cols, opts.rows);
            
    resource->plane = ncplane_create(stdplane, &opts);
    if (!resource->plane) {
        log_msg(ERROR, "media_output", "Failed to create plane for image at (%d, %d)", 
                resource->options.x, resource->options.y);
        return false;
    }

    // Set up visual options for direct blitting
    struct ncvisual_options vopts = {0};
    vopts.n = resource->plane;
    vopts.y = 0;  // Relative to the plane
    vopts.x = 0;  // Relative to the plane
    vopts.scaling = resource->options.scaling;
    vopts.blitter = NCBLIT_2x1;  // Simple blitter that works better

    // Use direct blit
    if (!ncvisual_blit(nc, resource->visual, &vopts)) {
        log_msg(ERROR, "media_output", "Failed to blit visual to plane");
        ncplane_destroy(resource->plane);
        resource->plane = NULL;
        return false;
    }

    // Make sure the plane is visible
    ncplane_move_top(resource->plane);

    // Make sure changes are visible - force a render
    log_msg(INFO, "media_output", "Rendering frame to display visual");
    notcurses_render(nc);  // Directly call notcurses_render for maximum compatibility
    
    log_msg(INFO, "media_output", "Successfully displayed image");
    return true;
}

/* =========================================================================
 * ANIMATION DISPLAY FUNCTIONS
 * ========================================================================= */

// Display a GIF file at specified coordinates with scaling
bool display_animation_at(const char* filename, int x, int y, int height, int width, scale_type_t scale_type, float fps, bool loop) {
    // Validate parameters
    if (!filename || height <= 0) {
        log_msg(ERROR, "media_output", "Invalid parameters for display_gif_at");
        return false;
    }
    loaded_visual_t* resource = ready_media(filename, x, y, height, width, scale_type);
    if (!resource) {
        log_msg(ERROR, "media_output", "Failed to load GIF for display");
        return false;
    }
    return display_animation(resource, fps, loop);
}

// Fill the background with a GIF file scaled to terminal size
bool fill_background_with_animation(const char* filename, float fps, bool loop) {
    // Validate parameters
    if (!filename) {
        log_msg(ERROR, "media_output", "Invalid filename for fill_background_with_gif");
        return false;
    }
    loaded_visual_t* resource = ready_media(filename, 0, 0, 0, 0, SCALE_FULLSCREEN);
    if (!resource) {
        log_msg(ERROR, "media_output", "Failed to load GIF for background");
        return false;
    }
    return display_animation(resource, fps, loop);
}

// Helper function for displaying animations
static bool display_animation(loaded_visual_t* resource, float fps, bool loop) {
    // Validate parameters
    if (!resource || !resource->visual) {
        log_msg(ERROR, "media_output", "Invalid parameters for display_animation");
        return false;
    }
    if (resource->media_type != MEDIA_GIF) {
        log_msg(ERROR, "media_output", "Invalid media type for animation");
        return false;
    }
    if (resource->is_playing) {
        log_msg(INFO, "media_output", "Animation already playing");
        // Stop the existing animation before starting a new one
        if (resource->visual) {
            ncvisual_destroy(resource->visual);
            resource->visual = NULL;
        }
        if (resource->plane) {
            ncplane_destroy(resource->plane);
            resource->plane = NULL;
        }
        resource->is_playing = false;
        
        // Reload the visual from file
        char* filepath = build_filepath(resource->path, resource->media_type);
        if (filepath) {
            resource->visual = ncvisual_from_file(filepath);
            free(filepath);
            if (!resource->visual) {
                log_msg(ERROR, "media_output", "Failed to reload visual");
                return false;
            }
        }
    }
    
    // Clean up existing plane if needed
    if (resource->plane) {
        ncplane_destroy(resource->plane);
        resource->plane = NULL;
    }
    
    // Create a new plane for the visual with appropriate dimensions
    struct ncplane_options options = {0};
    options.y = resource->options.y;
    options.x = resource->options.x;
    options.rows = resource->options.leny > 0 ? resource->options.leny : resource->height;
    options.cols = resource->options.lenx > 0 ? resource->options.lenx : resource->width;

    log_msg(INFO, "media_output", "Creating animation plane at (%d,%d) size %dx%d", 
            options.x, options.y, options.cols, options.rows);
            
    resource->plane = ncplane_create(stdplane, &options);
    if (!resource->plane) {
        log_msg(ERROR, "media_output", "Failed to create plane for animation");
        return false;
    }
    
    // Set up visual options for streaming
    struct ncvisual_options vopts = {0};
    vopts.n = resource->plane;         // Use our created plane
    vopts.scaling = resource->options.scaling;
    vopts.y = 0;                       // Coordinates relative to plane
    vopts.x = 0;
    vopts.blitter = NCBLIT_2x1;        // Use reliable blitter
    
    // Move the plane to the top to ensure visibility
    ncplane_move_top(resource->plane);

    // Mark as playing
    resource->is_playing = true;

    // Start the animation - negative iterations means infinite loop
    int iterations = loop ? -1 : 1;
    
    // Set the FPS via a timespec (converting fps to nanoseconds)
    resource->options.blitter = NCBLIT_2x1;  // Override blitter in original options too
    
    // Use our constructed options instead of resource->options for the stream
    int ret = ncvisual_stream(nc, resource->visual, iterations, animation_callback, &vopts, resource);

    if (ret) {
        log_msg(ERROR, "media_output", "Failed to start animation: error code %d", ret);
        resource->is_playing = false;
        if (resource->plane) {
            ncplane_destroy(resource->plane);
            resource->plane = NULL;
        }
        return false;
    }

    // Force an initial render to make the animation appear immediately
    notcurses_render(nc);
    
    log_msg(INFO, "media_output", "Successfully displaying animation");
    return true;
}

/* =========================================================================
 * GENERAL LOAD AND READY FUNCTIONS
 * ========================================================================= */

// Load a media resource
static loaded_visual_t* load_media(const char* filename, media_type_t media_type) {
    // Validate parameters
    if (!filename) {
        log_msg(ERROR, "media_output", "Invalid filename for load_media");
        return NULL;
    }

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

static loaded_visual_t* ready_media(const char* filename, int x, int y, int height, int width, scale_type_t scale_type) {
    // Validate parameters
    if (!filename || height < 0) { // Allow height=0 for fullscreen scaling
        log_msg(ERROR, "media_output", "Invalid parameters for display_media");
        return NULL;
    }

    // Determine media media_type from extension
    media_type_t media_type;
    if (is_file_extension(filename, ".png")) {
        media_type = MEDIA_PNG;
    } else if (is_file_extension(filename, ".gif")) {
        media_type = MEDIA_GIF;
    } else if (is_file_extension(filename, ".mp4")) {
        media_type = MEDIA_MP4;
    } else {
        return NULL; // Unsupported file media_type
    }

    // load the media
    loaded_visual_t* resource = load_media(filename, media_type);
    if (!resource || !resource->is_loaded) {
        log_msg(ERROR, "media_output", "Failed to load media: %s", filename);
        return NULL;
    }
    
    // do scaling
    setup_scaling_options(resource, scale_type, width, height);

    // coordinates
    resource->options.y = y;
    resource->options.x = x;

    // Clean up existing plane if needed
    if (resource->plane) {
        log_msg(INFO, "media_output", "Destroying existing plane before creating new one");
        ncplane_destroy(resource->plane);
        resource->plane = NULL;
    }

    log_msg(INFO, "media_output", "Media successfully readied for display");
    return resource;
}

/* =========================================================================
 * ANIMATION CONTROL FUNCTIONS
 * ========================================================================= */

// Stop playing an animated media (GIF or MP4)
bool stop_animation(const char* filename) {
    if (!filename) {
        return false;
    }
    
    // First try to build the filepath to match against full paths
    media_type_t media_type = MEDIA_GIF;
    if (is_file_extension(filename, ".png")) {
        media_type = MEDIA_PNG;
    } else if (is_file_extension(filename, ".mp4")) {
        media_type = MEDIA_MP4;
    }
    
    char* filepath = build_filepath(filename, media_type);
    bool found = false;
    
    // Look for the resource in our cache
    for (int i = 0; i < resource_count; i++) {
        // Check if it matches the full path or just the filename
        if ((filepath && strcmp(resources[i].path, filepath) == 0 && resources[i].is_playing) ||
            (strstr(resources[i].path, filename) != NULL && resources[i].is_playing)) {
            // Stop the animation by closing the visual
            struct ncvisual* visual = (struct ncvisual*)resources[i].visual;
            ncvisual_destroy(visual);
            resources[i].visual = NULL;
            resources[i].plane = NULL;
            resources[i].is_playing = false;
            resources[i].is_loaded = false;
            found = true;
            break;
        }
    }
    
    if (filepath) {
        free(filepath);
    }
    
    return found;
}

// Stop all animations
void stop_all_animations(void) {
    for (int i = 0; i < resource_count; i++) {
        if (resources[i].is_playing) {
            struct ncvisual* visual = (struct ncvisual*)resources[i].visual;
            ncvisual_destroy(visual);
            resources[i].visual = NULL;
            resources[i].plane = NULL;
            resources[i].is_playing = false;
            resources[i].is_loaded = false;
        }
    }
}

// Refresh media display
bool refresh_media_display(void) {
    // Force a redraw of the terminal
    return notcurses_render(nc);
}

// Helper function for animation callback
static int animation_callback(struct ncvisual* ncv, struct ncvisual_options* vopts,
                              const struct timespec* abstime, void* data) {
    loaded_visual_t* visual = (loaded_visual_t*) data;

    // Check if we should stop playing
    if (!visual || !visual->is_playing) {
        return -1;// Stop the animation
    }

    // Render the current frame
    vopts->n = visual->plane;

    if (!ncvisual_blit(nc, ncv, vopts)) {
        log_msg(ERROR, "media_output", "Failed to render animation frame");
        return -1;
    }

    // Render the changes
    if (!notcurses_render(nc)) {
        log_msg(ERROR, "media_output", "Failed to render animation changes");
        return -1;
    }

    return 0;// Continue playing
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
    if (!filename) {
        return false;
    }
    
    // Determine media media_type from extension
    media_type_t media_type;
    if (is_file_extension(filename, ".png")) {
        media_type = MEDIA_PNG;
    } else if (is_file_extension(filename, ".gif")) {
        media_type = MEDIA_GIF;
    } else if (is_file_extension(filename, ".mp4")) {
        media_type = MEDIA_MP4;
    } else {
        log_msg(ERROR, "media_output", "Unsupported file type for %s", filename);
        return false; // Unsupported file media_type
    }
    
    // Load the media and add to cache
    loaded_visual_t* resource = load_media(filename, media_type);
    return resource != NULL && resource->is_loaded;
}

// Reload media after terminal resize
bool reload_media_after_resize(void) {
    // For any displayed media, we'd need to reload/rescale it
    // For simplicity, we'll just unload all resources and let them be reloaded on next access
    media_cleanup();
    return true;
}

/* =========================================================================
 * INTERNAL UTILITY FUNCTIONS
 * ========================================================================= */

 // Helper function to get cell dimensions
static bool get_cell_dimensions(int* cell_width, int* cell_height) {
    if (!stdplane || !cell_width || !cell_height) {
        log_msg(ERROR, "media_output", "Cannot get cell dimensions");
        return false;
    }

    unsigned y, x;
    ncplane_dim_yx(stdplane, &y, &x);
    *cell_height = y > 0 ? y : 24;// Fallback values
    *cell_width = x > 0 ? x : 80; // if we can't get dimensions

    return true;
}

// Helper function to set up scaling options
static void setup_scaling_options(loaded_visual_t* visual, scale_type_t scale_type,
                                  int target_width, int target_height) {
    // Apply scaling based on media_type
    switch (scale_type) {
        case SCALE_PRESERVE:
            // Scale preserving aspect ratio
            visual->options.scaling = NCSCALE_SCALE;
            // Use original height and width if not specified
            visual->options.leny = target_height > 0 ? target_height : visual->height;
            visual->options.lenx = target_width > 0 ? target_width : visual->width;
            break;

        case SCALE_STRETCH:
            // Stretch to exact dimensions
            visual->options.scaling = NCSCALE_STRETCH;
            // Use original height and width if not specified
            visual->options.leny = target_height > 0 ? target_height : visual->height;
            visual->options.lenx = target_width > 0 ? target_width : visual->width;
            break;

        case SCALE_CELL:
            // Scale to fit in a single cell
            visual->options.scaling = NCSCALE_SCALE;
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
            } else {
                // Fallback to original dimensions
                visual->options.leny = visual->height;
                visual->options.lenx = visual->width;
            }
            break;

        case SCALE_NONE:
        default:
            // No scaling
            visual->options.scaling = NCSCALE_NONE;
            break;
    }
}

// Check if a filename has a specific extension
static bool is_file_extension(const char* filename, const char* extension) {
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

// Ensure a directory exists
static bool ensure_directory_exists(const char* path) {
    // This is a simplified implementation
    // In a real implementation, you would check if the directory exists
    // and create it if it doesn't
    return true;
}

// Build a full file path
static char* build_filepath(const char* filename, media_type_t media_type) {
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

// Free a media resource
static void free_media_resource(loaded_visual_t* resource) {
    if (!resource || !resource->is_loaded) {
        return;
    }

    resource->is_loaded = false;
    resource->is_playing = false;
    
    // Clean up the plane properly - first erase its contents
    if (resource->plane) {
        log_msg(INFO, "media_output", "Erasing plane contents before destruction");

        // Erase the plane contents with transparency
        ncplane_erase(resource->plane);

        // Make sure the erase is visible
        render_frame();

        // Then destroy the plane
        log_msg(INFO, "media_output", "Destroying plane");
        ncplane_destroy(resource->plane);
    }

    // Free the visual resource
    if (resource->visual) {
        log_msg(INFO, "media_output", "Destroying visual");
        ncvisual_destroy(resource->visual);
    }

    // Free path if allocated
    if (resource->path) {
        free(resource->path);
    }

    // Resource is part of static array, no need to free the structure itself
    
    log_msg(INFO, "media_output", "Visual resources cleanup complete");
    
    
}

static void media_cleanup(void) {
    // Free all loaded resources
    for (int i = 0; i < resource_count; i++) {
        if (resources[i].is_loaded) {
            free_media_resource(&resources[i]);
        }
    }
    resource_count = 0;
}



