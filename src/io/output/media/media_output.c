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

// Global variables
#define MAX_RESOURCES 128
static media_resource_t resources[MAX_RESOURCES];
static int resource_count = 0;

/* =========================================================================
 * FORWARD DECLARATIONS
 * ========================================================================= */

// Forward declarations for internal functions
static bool is_file_extension(const char* filename, const char* extension);
static media_resource_t* load_media(const char* filename, media_type_t type);
static bool ensure_directory_exists(const char* path);
static char* build_filepath(const char* filename, media_type_t type);
static void free_media_resource(media_resource_t* resource);
static void media_cleanup(void);

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
 * UTILITY FUNCTIONS FOR RENDERING OPTIONS
 * ========================================================================= */

// Create visual options structure
struct ncvisual_options create_visual_options(struct ncplane* target_plane, 
                                             ncscale_e scaling,
                                             int y, int x,
                                             int height, int width,
                                             ncblitter_e blitter,
                                             uint64_t flags) {
    struct ncvisual_options vopts = {
        .n = target_plane,                 // Target plane
        .scaling = scaling,// Scaling mode
        .y = y,                            // Y coordinate
        .x = x,                            // X coordinate
        .begy = 0,                         // Begin Y in source
        .begx = 0,                         // Begin X in source
        .leny = height,                    // Height (in terminal cells)
        .lenx = width,                     // Width (in terminal cells)
        .blitter = blitter,                // Blitter
        .flags = flags,                    // Flags
        .transcolor = 0,                   // No transparent color
    };
    
    return vopts;
}

// Create dynamically allocated visual options structure
struct ncvisual_options* create_visual_options_alloc(struct ncplane* target_plane, 
                                                    ncscale_e scaling,
                                                    int y, int x,
                                                    int height, int width,
                                                    ncblitter_e blitter,
                                                    uint64_t flags) {
    struct ncvisual_options* vopts = malloc(sizeof(struct ncvisual_options));
    if (!vopts) {
        return NULL;
    }
    
    *vopts = create_visual_options(target_plane, scaling, y, x, height, width, blitter, flags);
    return vopts;
}

/* =========================================================================
 * PNG DISPLAY FUNCTIONS
 * ========================================================================= */

// Display a PNG file at specified coordinates with scaling
bool display_png_at(const char* filename, int x, int y, int height, ncscale_e scaling) {
    // Validate parameters
    if (!filename || height <= 0) {
        return false;
    }
    
    // Load the media resource
    media_resource_t* resource = load_media(filename, MEDIA_PNG);
    if (!resource || !resource->is_loaded) {
        return false;
    }
    
    // Get the ncvisual from the resource
    struct ncvisual* visual = (struct ncvisual*)resource->data;
    if (!visual) {
        return false;
    }
    
    // Create options structure for rendering
    struct ncvisual_options vopts = create_visual_options(NULL, scaling, y, x, height, 0, NCBLIT_DEFAULT, 0);
    
    // Use ncvisual_blit instead of the obsolete ncvisual_render
    struct ncplane* created_plane = ncvisual_blit(nc, visual, &vopts);
    
    return created_plane != NULL;
}

// Fill the background with a PNG file scaled to terminal size
bool fill_background_with_png(const char* filename) {
    // Validate parameters
    if (!filename) {
        return false;
    }
    
    // Load the media resource
    media_resource_t* resource = load_media(filename, MEDIA_PNG);
    if (!resource || !resource->is_loaded) {
        return false;
    }
    
    // Get the ncvisual from the resource
    struct ncvisual* visual = (struct ncvisual*)resource->data;
    if (!visual) {
        return false;
    }
    
    // Get terminal dimensions
    int dim_y, dim_x;
    notcurses_term_dim_yx(nc, &dim_y, &dim_x);
    
    // Create a background plane beneath everything using ncplane_create instead of ncplane_new
    struct ncplane_options nopts = {
        .y = 0,
        .x = 0,
        .rows = dim_y,
        .cols = dim_x,
        .userptr = NULL,
        .name = "background",
        .resizecb = NULL,
        .flags = 0,
    };
    struct ncplane* bgplane = ncplane_create(stdplane, &nopts);
    
    if (!bgplane) {
        return false;
    }
    
    // Move it to the bottom of the z-order
    ncplane_move_bottom(bgplane);
    
    // Create options structure for rendering
    struct ncvisual_options vopts = create_visual_options(bgplane, NCSCALE_STRETCH, 0, 0, dim_y, dim_x, NCBLIT_DEFAULT, 0);
    
    // Use ncvisual_blit instead of the obsolete ncvisual_render
    return ncvisual_blit(nc, visual, &vopts) != NULL;
}

// Fill a single terminal cell with a PNG image
bool fill_cell_with_png(const char* filename, int y, int x) {
    // Validate parameters
    if (!filename) {
        return false;
    }
    
    // Load the media resource
    media_resource_t* resource = load_media(filename, MEDIA_PNG);
    if (!resource || !resource->is_loaded) {
        return false;
    }
    
    // Get the ncvisual from the resource
    struct ncvisual* visual = (struct ncvisual*)resource->data;
    if (!visual) {
        return false;
    }
    
    // Create options structure for rendering
    struct ncvisual_options vopts = create_visual_options(stdplane, NCSCALE_NONE, y, x, 1, 1, NCBLIT_PIXEL, 0);
    
    // Use ncvisual_blit instead of the obsolete ncvisual_render
    return ncvisual_blit(nc, visual, &vopts) != NULL;
}

/* =========================================================================
 * GIF DISPLAY FUNCTIONS
 * ========================================================================= */

// Display a GIF file at specified coordinates with scaling
bool display_gif_at(const char* filename, int x, int y, int height, ncscale_e scaling, float fps) {
    // Validate parameters
    if (!filename || height <= 0) {
        return false;
    }
    
    // Load the media resource
    media_resource_t* resource = load_media(filename, MEDIA_GIF);
    if (!resource || !resource->is_loaded) {
        return false;
    }
    
    // Get the ncvisual from the resource
    struct ncvisual* visual = (struct ncvisual*)resource->data;
    if (!visual) {
        return false;
    }
    
    // Create options structure for rendering
    struct ncvisual_options vopts = create_visual_options(NULL, scaling, y, x, height, 0, NCBLIT_DEFAULT, 0);
    
    // Start the visualization with direct streaming (no callback)
    struct ncplane* ncp = ncvisual_stream(nc, visual, fps > 0 ? fps : 0.1, NULL, &vopts, NULL);
    if (!ncp) {
        return false;
    }
    
    resource->is_playing = true;
    return true;
}

// Fill the background with a GIF file scaled to terminal size
bool fill_background_with_gif(const char* filename, float fps) {
    // Validate parameters
    if (!filename) {
        return false;
    }
    
    // Load the media resource
    media_resource_t* resource = load_media(filename, MEDIA_GIF);
    if (!resource || !resource->is_loaded) {
        return false;
    }
    
    // Get the ncvisual from the resource
    struct ncvisual* visual = (struct ncvisual*)resource->data;
    if (!visual) {
        return false;
    }
    
    // Get terminal dimensions
    int dim_y, dim_x;
    notcurses_term_dim_yx(nc, &dim_y, &dim_x);
    
    // Create a background plane beneath everything using ncplane_create instead of ncplane_new
    struct ncplane_options nopts = {
        .y = 0,
        .x = 0,
        .rows = dim_y,
        .cols = dim_x,
        .userptr = NULL,
        .name = "background",
        .resizecb = NULL,
        .flags = 0,
    };
    struct ncplane* bgplane = ncplane_create(stdplane, &nopts);
    
    if (!bgplane) {
        return false;
    }
    
    // Move it to the bottom of the z-order
    ncplane_move_bottom(bgplane);
    
    // Create options structure for rendering
    struct ncvisual_options vopts = create_visual_options(bgplane, NCSCALE_STRETCH, 0, 0, dim_y, dim_x, NCBLIT_DEFAULT, 0);
    
    // Start the visualization with direct streaming (no callback)
    struct ncplane* ncp = ncvisual_stream(nc, visual, fps > 0 ? fps : 0.1, NULL, &vopts, NULL);
    if (!ncp) {
        ncplane_destroy(bgplane);
        return false;
    }
    
    resource->is_playing = true;
    return true;
}

/* =========================================================================
 * MP4 DISPLAY FUNCTIONS
 * ========================================================================= */

// Display an MP4 file at specified coordinates with scaling
bool display_mp4_at(const char* filename, int x, int y, int height, ncscale_e scaling, float fps) {
    // The implementation is very similar to GIF, since notcurses treats them similarly
    return display_gif_at(filename, x, y, height, scaling, fps); // Reuse GIF implementation
}

// Fill the background with an MP4 file scaled to terminal size
bool fill_background_with_mp4(const char* filename, float fps) {
    // The implementation is very similar to GIF, since notcurses treats them similarly
    return fill_background_with_gif(filename, fps); // Reuse GIF implementation
}

/* =========================================================================
 * ANIMATION CONTROL FUNCTIONS
 * ========================================================================= */

// Stop playing an animated media (GIF or MP4)
bool stop_animation(const char* filename) {
    if (!filename) {
        return false;
    }
    
    // Look for the resource in our cache
    for (int i = 0; i < resource_count; i++) {
        if (strcmp(resources[i].filepath, filename) == 0 && resources[i].is_playing) {
            // Stop the animation by closing the visual
            struct ncvisual* visual = (struct ncvisual*)resources[i].data;
            ncvisual_destroy(visual);
            resources[i].data = NULL;
            resources[i].is_playing = false;
            resources[i].is_loaded = false;
            return true;
        }
    }
    
    return false;
}

// Stop all animations
void stop_all_animations(void) {
    for (int i = 0; i < resource_count; i++) {
        if (resources[i].is_playing) {
            struct ncvisual* visual = (struct ncvisual*)resources[i].data;
            ncvisual_destroy(visual);
            resources[i].data = NULL;
            resources[i].is_playing = false;
            resources[i].is_loaded = false;
        }
    }
}

// Pause/resume an animation
bool toggle_animation_pause(const char* filename) {
    // This would require more complex state tracking
    // For simplicity, we'll just indicate that this is not implemented yet
    // A real implementation would need to pause the ncvisual_stream
    return false;
}

// Refresh media display
bool refresh_media_display(void) {
    // Force a redraw of the terminal
    return notcurses_render(nc);
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
        if (strcmp(resources[i].filepath, filename) == 0) {
            free_media_resource(&resources[i]);
            
            // Shift other resources if this isn't the last one
            if (i < resource_count - 1) {
                memmove(&resources[i], &resources[i + 1], (resource_count - i - 1) * sizeof(media_resource_t));
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
    
    // Determine media type from extension
    media_type_t type;
    if (is_file_extension(filename, ".png")) {
        type = MEDIA_PNG;
    } else if (is_file_extension(filename, ".gif")) {
        type = MEDIA_GIF;
    } else if (is_file_extension(filename, ".mp4")) {
        type = MEDIA_MP4;
    } else {
        return false; // Unsupported file type
    }
    
    // Load the media and add to cache
    media_resource_t* resource = load_media(filename, type);
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

// Load a media resource
static media_resource_t* load_media(const char* filename, media_type_t type) {
    // Build the full file path
    char* filepath = build_filepath(filename, type);
    if (!filepath) {
        return NULL;
    }
    
    // Check if we already have this resource loaded
    for (int i = 0; i < resource_count; i++) {
        if (strcmp(resources[i].filepath, filepath) == 0) {
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
    
    // Allocate a new resource
    media_resource_t* resource = &resources[resource_count];
    resource_count++;
    
    // Initialize the new resource
    resource->type = type;
    resource->is_loaded = false;
    resource->is_playing = false;
    strncpy(resource->filepath, filepath, MAX_PATH_LEN - 1);
    resource->filepath[MAX_PATH_LEN - 1] = '\0';
    
    // Load the visual based on the media type
    struct ncvisual* visual = ncvisual_from_file(filepath);
    if (!visual) {
        // Loading failed, decrement count
        resource_count--;
        free(filepath);
        return NULL;
    }
    
    // Store the visual in the resource
    resource->data = visual;
    resource->is_loaded = true;
    
    free(filepath);
    return resource;
}

// Ensure a directory exists
static bool ensure_directory_exists(const char* path) {
    // This is a simplified implementation
    // In a real implementation, you would check if the directory exists
    // and create it if it doesn't
    return true;
}

// Build a full file path
static char* build_filepath(const char* filename, media_type_t type) {
    // Allocate memory for the path
    char* filepath = malloc(MAX_PATH_LEN);
    if (!filepath) {
        return NULL;
    }
    
    // Build the path based on the media type
    const char* subdir = "";
    switch (type) {
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
    
    return filepath;
}

// Free a media resource
static void free_media_resource(media_resource_t* resource) {
    if (!resource || !resource->is_loaded) {
        return;
    }
    
    // Free the data based on the media type
    switch (resource->type) {
        case MEDIA_PNG:
        case MEDIA_GIF:
        case MEDIA_MP4:
            ncvisual_destroy((struct ncvisual*)resource->data);
            break;
    }
    
    // Reset the resource
    resource->data = NULL;
    resource->is_loaded = false;
    resource->is_playing = false;
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



