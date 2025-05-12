#include "media_output.h"

#include "../../../logging/logger.h"
#include "../../io_handler.h"  // Include this to access global nc and stdplane
#include "../common/common_output.h"  // For get_screen_dimensions and render_frame

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Opaque structure for loaded visuals
struct loaded_visual_s {
    struct ncvisual* visual;         // The Notcurses visual
    struct ncplane* plane;           // The plane for rendering the visual
    struct ncvisual_options options; // Display options
    int width;                       // Original width
    int height;                      // Original height
    int frames;                      // Number of frames (for animations)
    bool is_playing;                 // Whether an animation is currently playing
    char* path;                      // Path to the file (for reloading)
};

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

    log_msg(INFO, "media_output", "Media output handler initialized");
    return true;
}

// Helper function to get cell dimensions
static bool get_cell_dimensions(int* cell_width, int* cell_height) {
    if (!stdplane || !cell_width || !cell_height) {
        log_msg(ERROR, "media_output", "Cannot get cell dimensions");
        return false;
    }

    unsigned y, x;
    ncplane_dim_yx(stdplane, &y, &x);
    *cell_height = y > 0 ? y : 24;  // Fallback values
    *cell_width = x > 0 ? x : 80;   // if we can't get dimensions
    
    return true;
}

loaded_visual_t* load_image(const char* path, int* width, int* height) {
    if (!nc) {
        log_msg(ERROR, "media_output", "Notcurses not initialized");
        return NULL;
    }

    if (!path) {
        log_msg(ERROR, "media_output", "Invalid image path (NULL)");
        return NULL;
    }

    log_msg(INFO, "media_output", "Attempting to load image from: %s", path);

    // Load the visual from file with detailed error handling
    struct ncvisual* visual = ncvisual_from_file(path);
    if (!visual) {
        log_msg(ERROR, "media_output", "Failed to load image: %s (check if file exists)", path);
        return NULL;
    }

    // Create the loaded_visual structure
    loaded_visual_t* loaded = malloc(sizeof(loaded_visual_t));
    if (!loaded) {
        log_msg(ERROR, "media_output", "Failed to allocate memory for loaded visual");
        ncvisual_destroy(visual);
        return NULL;
    }

    // Initialize the structure
    memset(loaded, 0, sizeof(loaded_visual_t));
    loaded->visual = visual;
    loaded->plane = NULL; // Will be created when displayed

    // Get visual dimensions
    struct ncvisual_options vopts = {0};
    struct ncvgeom geom = {0};
    int geo_ret = ncvisual_geom(nc, visual, &vopts, &geom);

    if (geo_ret) {
        log_msg(WARNING, "media_output", "Failed to get visual geometry, using default dimensions");
        // Use defaults if we can't get the dimensions
        loaded->width = 20;  // Default width
        loaded->height = 10; // Default height
    } else {
        // Store dimensions with detailed logging
        if (geom.pixx <= 0 || geom.pixy <= 0) {
            log_msg(WARNING, "media_output", "Invalid image dimensions: %dx%d, using defaults",
                    geom.pixx, geom.pixy);
            loaded->width = 20;  // Default width
            loaded->height = 10; // Default height
        } else {
            loaded->width = geom.pixx;  // Width in pixels
            loaded->height = geom.pixy; // Height in pixels
            log_msg(INFO, "media_output", "Got image dimensions: %dx%d", loaded->width, loaded->height);
        }
    }

    // Save the path for potential reloading
    loaded->path = strdup(path);
    if (!loaded->path) {
        log_msg(ERROR, "media_output", "Failed to store path");
        ncvisual_destroy(visual);
        free(loaded);
        return NULL;
    }

    // Set output parameters if requested
    if (width) *width = loaded->width;
    if (height) *height = loaded->height;

    loaded->frames = 1; // Single frame for static images
    loaded->is_playing = false;

    log_msg(INFO, "media_output", "Successfully loaded image: %s (%dx%d)",
            path, loaded->width, loaded->height);
    return loaded;
}

loaded_visual_t* load_gif(const char* path, int* width, int* height, int* frames) {
    if (!nc || !path) {
        log_msg(ERROR, "media_output", "Media output not initialized or invalid path");
        return NULL;
    }

    // Load the visual from file
    struct ncvisual* visual = ncvisual_from_file(path);
    if (!visual) {
        log_msg(ERROR, "media_output", "Failed to load GIF: %s", path);
        return NULL;
    }

    // Create the loaded_visual structure
    loaded_visual_t* loaded = malloc(sizeof(loaded_visual_t));
    if (!loaded) {
        log_msg(ERROR, "media_output", "Failed to allocate memory for loaded visual");
        ncvisual_destroy(visual);
        return NULL;
    }

    // Initialize the structure
    memset(loaded, 0, sizeof(loaded_visual_t));
    loaded->visual = visual;
    loaded->plane = NULL; // Will be created when displayed

    // Get visual dimensions
    struct ncvisual_options vopts = {0};
    struct ncvgeom geom = {0};
    ncvisual_geom(nc, visual, &vopts, &geom);
    
    // Store dimensions
    loaded->width = geom.pixy;  // Height in pixels
    loaded->height = geom.pixx; // Width in pixels

    // Save the path for potential reloading
    loaded->path = strdup(path);
    if (!loaded->path) {
        log_msg(ERROR, "media_output", "Failed to store path");
        ncvisual_destroy(visual);
        free(loaded);
        return NULL;
    }

    // Count frames by decoding the whole GIF
    loaded->frames = 0;
    while (ncvisual_decode(visual) != 1) {
        loaded->frames++;
    }

    // Reset to first frame
    ncvisual_destroy(visual);
    loaded->visual = ncvisual_from_file(path);
    if (!loaded->visual) {
        log_msg(ERROR, "media_output", "Failed to reload GIF after frame counting");
        free(loaded->path);
        free(loaded);
        return NULL;
    }

    // Set output parameters if requested
    if (width) *width = loaded->width;
    if (height) *height = loaded->height;
    if (frames) *frames = loaded->frames;

    loaded->is_playing = false;

    log_msg(INFO, "media_output", "Loaded GIF: %s (%dx%d, %d frames)",
            path, loaded->width, loaded->height, loaded->frames);
    return loaded;
}

// Helper function to set up scaling options
static void setup_scaling_options(loaded_visual_t* visual, int scale_type, 
                                 int target_width, int target_height) {
    // Apply scaling based on type
    switch (scale_type) {
        case SCALE_PRESERVE:
            // Scale preserving aspect ratio
            visual->options.scaling = NCSCALE_SCALE;
            visual->options.leny = target_height > 0 ? target_height : 0;
            visual->options.lenx = target_width > 0 ? target_width : 0;
            break;
            
        case SCALE_STRETCH:
            // Stretch to exact dimensions
            if (target_width > 0 && target_height > 0) {
                visual->options.scaling = NCSCALE_STRETCH;
                visual->options.leny = target_height;
                visual->options.lenx = target_width;
            } else {
                // Fall back to no scaling if dimensions are invalid
                visual->options.scaling = NCSCALE_NONE;
            }
            break;
            
        case SCALE_CELL:
            // Scale to fit in a single cell
            visual->options.scaling = NCSCALE_SCALE;
            visual->options.leny = 1;
            visual->options.lenx = 1;
            break;
            
        case SCALE_NONE:
        default:
            // No scaling
            visual->options.scaling = NCSCALE_NONE;
            break;
    }
}

bool display_visual(loaded_visual_t* visual, int y, int x, int scale_type,
                    int target_width, int target_height) {
    if (!nc) {
        log_msg(ERROR, "media_output", "Notcurses instance is NULL");
        return false;
    }

    if (!visual) {
        log_msg(ERROR, "media_output", "Visual pointer is NULL");
        return false;
    }

    if (!visual->visual) {
        log_msg(ERROR, "media_output", "Visual's ncvisual is NULL");
        return false;
    }

    // Stop any ongoing animation
    if (visual->is_playing) {
        log_msg(INFO, "media_output", "Stopping ongoing animation before displaying static image");
        stop_animated_visual(visual);
    }

    // Prepare display options
    memset(&visual->options, 0, sizeof(visual->options));
    visual->options.y = y;
    visual->options.x = x;

    // Handle different scaling modes with detailed logging
    log_msg(INFO, "media_output", "Preparing to display visual with scale_type=%d, target dimensions=%dx%d",
            scale_type, target_width, target_height);

    // Handle cell-based scaling
    if (scale_type == SCALE_CELL) {
        // For cell-based scaling, target width/height are ignored
        log_msg(INFO, "media_output", "Using cell-based scaling (1x1)");
        setup_scaling_options(visual, scale_type, 1, 1);
    } else {
        // For fullscreen mode, use screen dimensions
        if (target_width <= 0 && target_height <= 0 && scale_type != SCALE_NONE) {
            int screen_width, screen_height;
            if (get_screen_dimensions(&screen_width, &screen_height)) {
                log_msg(INFO, "media_output", "Using screen dimensions for scaling: %dx%d",
                        screen_width, screen_height);
                setup_scaling_options(visual, scale_type, screen_width, screen_height);
            } else {
                log_msg(WARNING, "media_output", "Failed to get screen dimensions, using provided dimensions");
                setup_scaling_options(visual, scale_type, target_width, target_height);
            }
        } else {
            // Normal case with provided dimensions
            log_msg(INFO, "media_output", "Using provided dimensions for scaling: %dx%d",
                    target_width, target_height);
            setup_scaling_options(visual, scale_type, target_width, target_height);
        }
    }

    // Clean up existing plane if needed
    if (visual->plane) {
        log_msg(INFO, "media_output", "Destroying existing plane before creating new one");
        ncplane_destroy(visual->plane);
        visual->plane = NULL;
    }

    // Try a simpler approach using NCScale
    visual->options.blitter = NCBLIT_2x2;  // Use a better blitter for clearer image
    visual->options.flags = NCVISUAL_OPTION_CHILDPLANE;  // Create as child plane

    // Set the plane to NULL to let ncvisual_blit create a new one
    visual->options.n = NULL;

    log_msg(INFO, "media_output", "Using simplified direct blit approach with 2x2 blitter");

    // Directly blit the visual to create a new plane
    struct ncplane* new_plane = ncvisual_blit(nc, visual->visual, &visual->options);

    if (!new_plane) {
        log_msg(ERROR, "media_output", "Failed to create new plane via direct blit");
        return false;
    }

    // Store the new plane
    if (visual->plane) {
        log_msg(INFO, "media_output", "Destroying old plane");
        ncplane_destroy(visual->plane);
    }
    visual->plane = new_plane;

    log_msg(INFO, "media_output", "Successfully created visual plane");

    // Make sure the plane is visible
    ncplane_move_top(visual->plane);

    // Make sure changes are visible - force a render
    log_msg(INFO, "media_output", "Rendering frame to display visual");
    if (!render_frame()) {
        log_msg(WARNING, "media_output", "Render frame failed, visual may not be visible");
    }

    log_msg(INFO, "media_output", "Visual successfully displayed");
    return true;
}

// Helper function for animation callback
static int animation_callback(struct ncvisual* ncv, struct ncvisual_options* vopts,
                              const struct timespec* abstime, void* data) {
    loaded_visual_t* visual = (loaded_visual_t*)data;

    // Check if we should stop playing
    if (!visual || !visual->is_playing) {
        return -1; // Stop the animation
    }

    // Render the current frame
    vopts->n = visual->plane;

    if (!ncvisual_blit(nc, ncv, vopts)) {
        log_msg(ERROR, "media_output", "Failed to render animation frame");
        return -1;
    }

    // Render the changes
    notcurses_render(nc);

    return 0; // Continue playing
}

bool play_animated_visual(loaded_visual_t* visual, int y, int x, int scale_type,
                          int target_width, int target_height, bool loop) {
    if (!nc || !visual || !visual->visual) {
        log_msg(ERROR, "media_output", "Invalid parameters for play_animated_visual");
        return false;
    }

    // Stop any ongoing animation
    if (visual->is_playing) {
        stop_animated_visual(visual);
    }

    // Reset to first frame if needed
    ncvisual_destroy(visual->visual);
    visual->visual = ncvisual_from_file(visual->path);
    if (!visual->visual) {
        log_msg(ERROR, "media_output", "Failed to reset visual for animation");
        return false;
    }

    // Prepare display options
    memset(&visual->options, 0, sizeof(visual->options));
    visual->options.y = y;
    visual->options.x = x;
    
    // Handle cell-based scaling
    if (scale_type == SCALE_CELL) {
        // For cell-based scaling, target width/height are ignored
        setup_scaling_options(visual, scale_type, 1, 1);
    } else {
        // For fullscreen mode, use screen dimensions
        if (target_width <= 0 && target_height <= 0 && scale_type != SCALE_NONE) {
            int screen_width, screen_height;
            if (get_screen_dimensions(&screen_width, &screen_height)) {
                setup_scaling_options(visual, scale_type, screen_width, screen_height);
            } else {
                setup_scaling_options(visual, scale_type, target_width, target_height);
            }
        } else {
            // Normal case with provided dimensions
            setup_scaling_options(visual, scale_type, target_width, target_height);
        }
    }

    // Clean up existing plane if needed
    if (visual->plane) {
        ncplane_destroy(visual->plane);
    }
    
    // Create a new plane for the visual
    ncplane_options options = {0};
    options.y = visual->options.y;
    options.x = visual->options.x;
    options.rows = visual->options.leny > 0 ? visual->options.leny : visual->height;
    options.cols = visual->options.lenx > 0 ? visual->options.lenx : visual->width;
    
    visual->plane = ncplane_create(stdplane, &options);
    if (!visual->plane) {
        log_msg(ERROR, "media_output", "Failed to create plane for animation");
        return false;
    }

    // Mark as playing
    visual->is_playing = true;

    // Start the animation - negative iterations means infinite loop
    int iterations = loop ? -1 : 1;
    int ret = ncvisual_stream(nc, visual->visual, iterations, animation_callback, 
                              &visual->options, visual);

    if (ret) {
        log_msg(ERROR, "media_output", "Failed to start animation");
        visual->is_playing = false;
        return false;
    }

    return true;
}

// Simplified functions for common display patterns

bool display_image_fullscreen(loaded_visual_t* visual) {
    int screen_width, screen_height;
    
    if (!get_screen_dimensions(&screen_width, &screen_height)) {
        log_msg(ERROR, "media_output", "Could not get screen dimensions");
        return false;
    }
    
    return display_visual(visual, 0, 0, SCALE_STRETCH, screen_width, screen_height);
}

bool display_image_positioned(loaded_visual_t* visual, int y, int x, int width, int height) {
    if (!visual) {
        log_msg(ERROR, "media_output", "Cannot display NULL visual");
        return false;
    }

    // Log the positioning information
    log_msg(INFO, "media_output", "Displaying image at position (%d, %d) with dimensions %dx%d", x, y, width, height);

    // Ensure we have reasonable dimensions
    if (width <= 0 || height <= 0) {
        log_msg(WARNING, "media_output", "Invalid dimensions for image display: %dx%d", width, height);
        // Use default dimensions based on screen size
        int screen_width, screen_height;
        if (get_screen_dimensions(&screen_width, &screen_height)) {
            width = screen_width / 4;  // Use 1/4 of screen width as default
            height = screen_height / 4; // Use 1/4 of screen height as default
            log_msg(INFO, "media_output", "Using default dimensions: %dx%d", width, height);
        } else {
            // Use hardcoded fallbacks if we can't get screen dimensions
            width = 20;
            height = 10;
            log_msg(INFO, "media_output", "Using fallback dimensions: %dx%d", width, height);
        }
    }

    // Try a direct approach instead of using display_visual
    if (visual->plane) {
        log_msg(INFO, "media_output", "Destroying existing plane");
        ncplane_destroy(visual->plane);
        visual->plane = NULL;
    }

    // Create a direct plane for the image
    struct ncplane_options opts = {0};
    opts.y = y;
    opts.x = x;
    opts.rows = height;
    opts.cols = width;

    visual->plane = ncplane_create(stdplane, &opts);
    if (!visual->plane) {
        log_msg(ERROR, "media_output", "Failed to create plane for image at (%d, %d)", x, y);
        return false;
    }

    // Setup visual options
    struct ncvisual_options vopts = {0};
    vopts.n = visual->plane;
    vopts.y = 0;  // Relative to the plane
    vopts.x = 0;  // Relative to the plane
    vopts.scaling = NCSCALE_STRETCH;
    vopts.blitter = NCBLIT_1x1;  // Simple blitter

    // Use direct blit instead of render (which doesn't exist in this notcurses version)
    struct ncplane* new_plane = ncvisual_blit(nc, visual->visual, &vopts);
    if (!new_plane) {
        log_msg(ERROR, "media_output", "Failed to blit visual to plane");
        ncplane_destroy(visual->plane);
        visual->plane = NULL;
        return false;
    }

    // Move the plane to the top
    ncplane_move_top(visual->plane);

    // Render the changes
    render_frame();

    log_msg(INFO, "media_output", "Successfully displayed image at (%d, %d) using direct rendering", x, y);
    return true;
}

bool display_image_cell(loaded_visual_t* visual, int y, int x) {
    return display_visual(visual, y, x, SCALE_CELL, 0, 0);
}

bool play_gif_fullscreen(loaded_visual_t* visual, bool loop) {
    int screen_width, screen_height;
    
    if (!get_screen_dimensions(&screen_width, &screen_height)) {
        log_msg(ERROR, "media_output", "Could not get screen dimensions");
        return false;
    }
    
    return play_animated_visual(visual, 0, 0, SCALE_STRETCH, screen_width, screen_height, loop);
}

bool play_gif_positioned(loaded_visual_t* visual, int y, int x, int width, int height, bool loop) {
    return play_animated_visual(visual, y, x, SCALE_STRETCH, width, height, loop);
}

bool play_gif_cell(loaded_visual_t* visual, int y, int x, bool loop) {
    return play_animated_visual(visual, y, x, SCALE_CELL, 0, 0, loop);
}

bool stop_animated_visual(loaded_visual_t* visual) {
    if (!visual) {
        log_msg(ERROR, "media_output", "Invalid visual for stop_animated_visual");
        return false;
    }

    visual->is_playing = false;
    return true;
}

// These functions are now defined in common_output.c
// We just use the ones from common_output.h here

void free_visual(loaded_visual_t* visual) {
    if (!visual) return;

    // Stop any playing animation
    if (visual->is_playing) {
        stop_animated_visual(visual);
    }

    // Free Notcurses resources
    if (visual->plane) {
        ncplane_destroy(visual->plane);
    }
    if (visual->visual) {
        ncvisual_destroy(visual->visual);
    }
    
    // Free path if allocated
    if (visual->path) {
        free(visual->path);
    }

    // Free the structure
    free(visual);
}

void shutdown_media_output(void) {
    // No specific resources to clean up here
    // All visual resources should be freed with free_visual()
    
    log_msg(INFO, "media_output", "Media output handler shut down");
}