#include "media_output.h"

#include "../../../logging/logger.h"

#include <stdlib.h>
#include <string.h>

// Opaque structure for loaded visuals
struct loaded_visual_s {
    struct ncvisual* visual;        // The Notcurses visual
    struct ncplane* plane;          // The plane for rendering the visual
    struct ncvisual_options options;// Display options
    int width;                      // Original width
    int height;                     // Original height
    int frames;                     // Number of frames (for animations)
    bool is_playing;                // Whether an animation is currently playing
};

// Global variables
static struct notcurses* nc = NULL;

bool init_media_output(struct notcurses* nc) {
    if (!nc) {
        log_msg(ERROR, "media_output", "Null Notcurses instance provided");
        return false;
    }

    nc = nc;
    log_msg(INFO, "media_output", "Media output handler initialized");
    return true;
}

loaded_visual_t* load_image(const char* path, int* width, int* height) {
    if (!nc || !path) {
        log_msg(ERROR, "media_output", "Media output not initialized or invalid path");
        return NULL;
    }

    // Load the visual from file
    struct ncvisual* visual = ncvisual_from_file(path);
    if (!visual) {
        log_msg(ERROR, "media_output", "Failed to load image: %s", path);
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
    loaded->plane = NULL;// Will be created when displayed

    // Get visual dimensions
    struct ncvisual_options vopts = {0};
    struct ncvgeom geom = {0};
    ncvisual_geom(nc, visual, &vopts, &geom);

    // Set output parameters if requested
    if (width) *width = loaded->width;
    if (height) *height = loaded->height;

    loaded->frames = 1;// Single frame for static images
    loaded->is_playing = false;

    log_msg(INFO, "media_output", "Loaded image: %s (%dx%d)", path, loaded->width, loaded->height);
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
    loaded->plane = NULL;// Will be created when displayed

    // Get visual dimensions
    struct ncvisual_options vopts = {0};
    struct ncvgeom geom = {0};
    ncvisual_geom(nc, visual, &vopts, &geom);

    // Count frames
    loaded->frames = 0;
    while (ncvisual_decode(visual) != 1) {
        loaded->frames++;
    }

    // Reset to first frame
    ncvisual_destroy(visual);
    loaded->visual = ncvisual_from_file(path);

    // Set output parameters if requested
    if (width) *width = loaded->width;
    if (height) *height = loaded->height;
    if (frames) *frames = loaded->frames;

    loaded->is_playing = false;

    log_msg(INFO, "media_output", "Loaded GIF: %s (%dx%d, %d frames)",
            path, loaded->width, loaded->height, loaded->frames);
    return loaded;
}

bool display_visual(loaded_visual_t* visual, int y, int x, int scale_type,
                    int target_width, int target_height) {
    if (!nc || !visual || !visual->visual) {
        log_msg(ERROR, "media_output", "Invalid parameters for display_visual");
        return false;
    }

    // Stop any ongoing animation
    if (visual->is_playing) {
        stop_animated_visual(visual);
    }

    // Prepare display options
    memset(&visual->options, 0, sizeof(visual->options));
    visual->options.y = y;
    visual->options.x = x;

    // Apply scaling if requested
    if (scale_type == 1 && (target_width > 0 || target_height > 0)) {
        // Scale preserving aspect ratio
        visual->options.scaling = NCSCALE_SCALE;
        visual->options.leny = target_height > 0 ? target_height : 0;
        visual->options.lenx = target_width > 0 ? target_width : 0;
    } else if (scale_type == 2 && target_width > 0 && target_height > 0) {
        // Stretch to exact dimensions
        visual->options.scaling = NCSCALE_STRETCH;
        visual->options.leny = target_height;
        visual->options.lenx = target_width;
    } else {
        // No scaling
        visual->options.scaling = NCSCALE_NONE;
    }

    // Create a new plane for the visual if needed
    if (visual->plane) {
        ncplane_destroy(visual->plane);
        visual->plane = NULL;
    }

    // Render the visual
    visual->plane = ncvisual_blit(nc, visual->visual, &visual->options);
    if (!visual->plane) {
        log_msg(ERROR, "media_output", "Failed to render visual");
        return false;
    }

    return true;
}

// Helper function for the animation thread
static int animation_callback(struct ncvisual* ncv, struct ncvisual_options* vopts,
                              const struct timespec* abstime, void* data) {
    loaded_visual_t* visual = (loaded_visual_t*) data;

    // Check if we should stop playing
    if (!visual || !visual->is_playing) {
        return -1;// Stop the animation
    }

    // Render the current frame
    vopts->n = visual->plane;

    // Critical section for rendering - use lock_output_mutex if it were accessible
    // In a full implementation, we would use the mutex from common_output.c
    // For now, since Notcurses manages this thread internally, we rely on its thread safety

    if (!ncvisual_blit(nc, ncv, vopts)) {
        log_msg(ERROR, "media_output", "Failed to render animation frame");
        return -1;
    }

    // Render the changes
    notcurses_render(nc);

    return 0;// Continue playing
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
    visual->visual = ncvisual_from_file(visual->visual);
    if (!visual->visual) {
        log_msg(ERROR, "media_output", "Failed to reset visual for animation");
        return false;
    }

    // Prepare display options
    memset(&visual->options, 0, sizeof(visual->options));
    visual->options.y = y;
    visual->options.x = x;

    // Apply scaling if requested
    if (scale_type == 1 && (target_width > 0 || target_height > 0)) {
        // Scale preserving aspect ratio
        visual->options.scaling = NCSCALE_SCALE;
        visual->options.leny = target_height > 0 ? target_height : 0;
        visual->options.lenx = target_width > 0 ? target_width : 0;
    } else if (scale_type == 2 && target_width > 0 && target_height > 0) {
        // Stretch to exact dimensions
        visual->options.scaling = NCSCALE_STRETCH;
        visual->options.leny = target_height;
        visual->options.lenx = target_width;
    } else {
        // No scaling
        visual->options.scaling = NCSCALE_NONE;
    }

    // Create a new plane for the visual
    if (visual->plane) {
        ncplane_destroy(visual->plane);
    }
    ncplane_options options = {0};// TODO define options
    visual->plane = ncplane_create(notcurses_stdplane(nc), &options);
    if (!visual->plane) {
        log_msg(ERROR, "media_output", "Failed to create plane for animation");
        return false;
    }

    // Mark as playing
    visual->is_playing = true;

    // Start the animation
    int ret = ncvisual_stream(nc, visual->visual,
                              loop ? -1 : 1,// negative value means infinite loop
                              animation_callback, &visual->options, visual);

    if (ret) {
        log_msg(ERROR, "media_output", "Failed to start animation");
        visual->is_playing = false;
        return false;
    }

    return true;
}

bool stop_animated_visual(loaded_visual_t* visual) {
    if (!visual) {
        log_msg(ERROR, "media_output", "Invalid visual for stop_animated_visual");
        return false;
    }

    // Mark as not playing to stop the animation thread
    visual->is_playing = false;

    return true;
}

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

    // Free the structure
    free(visual);
}

void shutdown_media_output(void) {
    nc = NULL;

    log_msg(INFO, "media_output", "Media output handler shut down");
}