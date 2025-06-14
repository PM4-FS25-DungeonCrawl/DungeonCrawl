/**
 * @file media_output.c
 * @brief Implements functionality for outputing media to the screen.
 */
#include "media_output.h"

#include "../../../logging/logger.h"
#include "../../input/input_handler.h"// For non-blocking input
#include "../../io_handler.h"         // Include this to access global nc and stdplane
#include "../common/output_handler.h" // For get_screen_dimensions and render_frame
#include "media_files.h"
#include "media_output_handler.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Include platform-specific headers for sleep functions
#ifndef _WIN32
    #include <unistd.h>// For usleep on Unix/Linux
#else
    #include <windows.h>// For Sleep on Windows
#endif


/* =========================================================================
 * FORWARD DECLARATIONS
 * ========================================================================= */

// Forward declarations for internal functions
static bool display_image(loaded_visual_t* resource);

static bool display_animation(loaded_visual_t* resource, float fps, bool loop);
static bool display_animation_interruptible(loaded_visual_t* resource, float fps, bool loop, input_event_t* interrupt_event);

static bool recreate_visual_for_loop(loaded_visual_t* resource);

/* =========================================================================
 * PNG DISPLAY FUNCTIONS
 * ========================================================================= */

// Display a PNG file at specified coordinates with scaling
bool display_image_at(const char* filename, int x, int y, int height, int width, scale_type_t scale_type) {
    // Validate parameters
    if (!filename || height < 0) {
        // Allow height=0 for automatic scaling
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

bool display_image_background(const char* filename) {
    if (!filename) {
        log_msg(ERROR, "media_output", "Invalid filename for display_image_background");
        return false;
    }

    // Get screen dimensions
    int width, height;
    if (!get_screen_dimensions(&width, &height)) {
        log_msg(ERROR, "media_output", "Failed to get screen dimensions");
        return false;
    }

    // Load and display the image at full screen size
    loaded_visual_t* resource = ready_media(filename, 0, 0, height, width, SCALE_FULLSCREEN);
    if (!resource) {
        log_msg(ERROR, "media_output", "Failed to load background image");
        return false;
    }

    return display_image(resource);
}

// Fill a single terminal cell with a PNG image
bool display_image_cell(const char* filename, int x, int y) {
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

/* =========================================================================
 * ANIMATION DISPLAY FUNCTIONS
 * ========================================================================= */

bool display_gif_at(const char* filename, int x, int y, int height, int width, scale_type_t scale_type, float fps, bool loop) {
    if (!filename || height < 0) {
        log_msg(ERROR, "media_output", "Invalid parameters for display_gif_at");
        return false;
    }

    loaded_visual_t* resource = ready_media(filename, x, y, height, width, scale_type);
    if (!resource) {
        log_msg(ERROR, "media_output", "Failed to load GIF for display");
        return false;
    }

    // Verify this is actually a GIF
    if (resource->media_type != MEDIA_GIF) {
        log_msg(ERROR, "media_output", "File %s is not a GIF", filename);
        return false;
    }

    return display_animation(resource, fps, loop);
}

bool display_gif_at_interruptible(const char* filename, int x, int y, int height, int width, scale_type_t scale_type, float fps, bool loop, input_event_t* interrupt_event) {
    if (!filename || height < 0) {
        log_msg(ERROR, "media_output", "Invalid parameters for display_gif_at_interruptible");
        return false;
    }

    loaded_visual_t* resource = ready_media(filename, x, y, height, width, scale_type);
    if (!resource) {
        log_msg(ERROR, "media_output", "Failed to load GIF for display");
        return false;
    }

    // Verify this is actually a GIF
    if (resource->media_type != MEDIA_GIF) {
        log_msg(ERROR, "media_output", "File %s is not a GIF", filename);
        return false;
    }

    return display_animation_interruptible(resource, fps, loop, interrupt_event);
}

bool display_gif_background(const char* filename, float fps, bool loop) {
    if (!filename) {
        log_msg(ERROR, "media_output", "Invalid filename for display_gif_background");
        return false;
    }

    // Get screen dimensions
    int width, height;
    if (!get_screen_dimensions(&width, &height)) {
        log_msg(ERROR, "media_output", "Failed to get screen dimensions");
        return false;
    }


    // Load and display the GIF at full screen size
    loaded_visual_t* resource = ready_media(filename, 0, 0, height, width, SCALE_FULLSCREEN);
    if (!resource) {
        log_msg(ERROR, "media_output", "Failed to load background GIF");
        return false;
    }

    // Verify this is actually a GIF
    if (resource->media_type != MEDIA_GIF) {
        log_msg(ERROR, "media_output", "File %s is not a GIF", filename);
        return false;
    }

    return display_animation(resource, fps, loop);
}


/* =========================================================================
 * ANIMATION CONTROL FUNCTIONS
 * ========================================================================= */

void media_output_play(loaded_visual_t* media) {
    if (!media) {
        log_msg(ERROR, "media_output", "Invalid media parameter for media_output_play");
        return;
    }

    media->is_playing = true;
    log_msg(DEBUG, "media_output", "Media playback started");
}

void media_output_pause(loaded_visual_t* media) {
    if (!media) {
        log_msg(ERROR, "media_output", "Invalid media parameter for media_output_pause");
        return;
    }

    media->is_playing = false;
    log_msg(DEBUG, "media_output", "Media playback paused");
}

void media_output_reset(loaded_visual_t* media) {
    if (!media) {
        log_msg(ERROR, "media_output", "Invalid media parameter for media_output_reset");
        return;
    }

    // Stop playback
    media->is_playing = false;

    // Reset the visual to the first frame if it's an animation or video
    if (media->visual && (media->media_type == MEDIA_GIF || media->media_type == MEDIA_MP4)) {
        // Reset to beginning by seeking to frame 0
        if (ncvisual_decode(media->visual) < 0) {
            log_msg(WARNING, "media_output", "Failed to reset visual to first frame");
        }
    }

    log_msg(DEBUG, "media_output", "Media reset to beginning");
}

/* =========================================================================
* INTERNAL DISPLAY FUNCTIONS
* ========================================================================= */

/**
 * @brief Helper function to display a visual.
 *
 * @param resource Visual to display.
 * @return True on success false on failure.
 */
static bool display_image(loaded_visual_t* resource) {
    // Validate parameters
    if (!resource || !resource->visual) {
        log_msg(ERROR, "media_output", "Invalid parameters for display_image");
        return false;
    }

    // Clean up existing plane if needed
    if (resource->plane) {
        ncplane_destroy(resource->plane);
        resource->plane = NULL;
    }

    // Create a direct plane for the image
    struct ncplane_options opts = {0};
    opts.y = resource->options.y;
    opts.x = resource->options.x;
    opts.rows = resource->options.leny > 0 ? resource->options.leny : resource->og_height;
    opts.cols = resource->options.lenx > 0 ? resource->options.lenx : resource->og_width;
    //opts.resizecb = standard_resize_callback;  // Add resize callback for proper handling

    resource->plane = ncplane_create(gio->stdplane, &opts);
    if (!resource->plane) {
        log_msg(ERROR, "media_output", "Failed to create plane for image at (%d, %d)",
                resource->options.x, resource->options.y);
        return false;
    }

    // Set up visual options for direct blitting
    struct ncvisual_options vopts = {0};
    vopts.n = resource->plane;
    vopts.y = 0;// Relative to the plane
    vopts.x = 0;// Relative to the plane
    vopts.scaling = resource->options.scaling;
    vopts.blitter = NCBLIT_2x2;// Simple blitter that works better

    // Use direct blit
    if (!ncvisual_blit(gio->nc, resource->visual, &vopts)) {
        log_msg(ERROR, "media_output", "Failed to blit visual to plane");
        ncplane_destroy(resource->plane);
        resource->plane = NULL;
        return false;
    }

    // Make sure the plane is visible
    ncplane_move_top(resource->plane);

    // Make sure changes are visible - force a render
    notcurses_render(gio->nc);// Directly call notcurses_render for maximum compatibility

    return true;
}

static bool display_animation(loaded_visual_t* resource, float fps, bool loop) {
    if (!resource || !resource->visual) {
        log_msg(ERROR, "media_output", "Invalid parameters for display_animation");
        return false;
    }

    // Validate fps
    if (fps <= 0.0f) {
        fps = 10.0f;// Default to 10 FPS
        log_msg(WARNING, "media_output", "Invalid FPS, using default 10 FPS");
    }

    // Calculate frame delay in microseconds
    int frame_delay_us = (int) (1000000.0f / fps);

    // Set up initial display (similar to display_image)
    if (resource->plane) {
        ncplane_destroy(resource->plane);
        resource->plane = NULL;
    }

    struct ncplane_options opts = {0};
    opts.y = resource->options.y;
    opts.x = resource->options.x;
    opts.rows = resource->options.leny > 0 ? resource->options.leny : resource->og_height;
    opts.cols = resource->options.lenx > 0 ? resource->options.lenx : resource->og_width;

    resource->plane = ncplane_create(gio->stdplane, &opts);
    if (!resource->plane) {
        log_msg(ERROR, "media_output", "Failed to create plane for animation at (%d, %d)",
                resource->options.x, resource->options.y);
        return false;
    }

    struct ncvisual_options vopts = {0};
    vopts.n = resource->plane;
    vopts.y = 0;
    vopts.x = 0;
    vopts.scaling = resource->options.scaling;
    vopts.blitter = NCBLIT_2x2;

    // Mark as playing
    resource->is_playing = true;

    // Animation loop
    do {
        // Clear the plane before rendering new frame
        ncplane_erase(resource->plane);

        // Render current frame
        if (!ncvisual_blit(gio->nc, resource->visual, &vopts)) {
            log_msg(ERROR, "media_output", "Failed to blit animation frame");
            resource->is_playing = false;
            return false;
        }

        // Make sure the plane is visible and render
        ncplane_move_top(resource->plane);
        notcurses_render(gio->nc);

        // Check for user input to allow interruption
        input_event_t input_event;
        if (get_input_nonblocking(&input_event)) {
            // Input detected - animation should be interrupted
            log_msg(DEBUG, "media_output", "Animation interrupted by user input");
            resource->is_playing = false;
            return true;// Return true to indicate successful completion (even though interrupted)
        }

        // Sleep for frame duration
#ifdef _WIN32
        Sleep(frame_delay_us / 1000);// Windows Sleep takes milliseconds
#else
        usleep(frame_delay_us);// Unix usleep takes microseconds
#endif

        // Try to decode next frame
        int decode_result = ncvisual_decode(resource->visual);
        if (decode_result == 1) {
            // End of animation reached
            if (loop) {
                // Reset to beginning for looping by recreating the visual
                log_msg(DEBUG, "media_output", "Animation loop completed, restarting");
                if (!recreate_visual_for_loop(resource)) {
                    log_msg(ERROR, "media_output", "Failed to recreate visual for looping");
                    resource->is_playing = false;
                    return false;
                }
                // Continue the loop with the newly recreated visual
                continue;
            } else {
                // Animation finished, not looping
                resource->is_playing = false;
                break;
            }
        } else if (decode_result < 0) {
            log_msg(ERROR, "media_output", "Error decoding animation frame");
            resource->is_playing = false;
            return false;
        }

        // Check if animation should continue playing
        // This allows external control via media_output_pause()
    } while (resource->is_playing);

    return true;
}

static bool display_animation_interruptible(loaded_visual_t* resource, float fps, bool loop, input_event_t* interrupt_event) {
    if (!resource || !resource->visual) {
        log_msg(ERROR, "media_output", "Invalid parameters for display_animation_interruptible");
        return false;
    }

    // Initialize interrupt_event if provided
    if (interrupt_event) {
        interrupt_event->type = INPUT_NONE;
    }

    // Validate fps
    if (fps <= 0.0f) {
        fps = 10.0f;// Default to 10 FPS
        log_msg(WARNING, "media_output", "Invalid FPS, using default 10 FPS");
    }

    // Calculate frame delay in microseconds
    int frame_delay_us = (int) (1000000.0f / fps);

    // Set up initial display (similar to display_image)
    if (resource->plane) {
        ncplane_destroy(resource->plane);
        resource->plane = NULL;
    }

    struct ncplane_options opts = {0};
    opts.y = resource->options.y;
    opts.x = resource->options.x;
    opts.rows = resource->options.leny > 0 ? resource->options.leny : resource->og_height;
    opts.cols = resource->options.lenx > 0 ? resource->options.lenx : resource->og_width;

    resource->plane = ncplane_create(gio->stdplane, &opts);
    if (!resource->plane) {
        log_msg(ERROR, "media_output", "Failed to create plane for animation at (%d, %d)",
                resource->options.x, resource->options.y);
        return false;
    }

    struct ncvisual_options vopts = {0};
    vopts.n = resource->plane;
    vopts.y = 0;
    vopts.x = 0;
    vopts.scaling = resource->options.scaling;
    vopts.blitter = NCBLIT_2x2;

    // Mark as playing
    resource->is_playing = true;

    // Animation loop
    do {
        // Clear the plane before rendering new frame
        ncplane_erase(resource->plane);

        // Render current frame
        if (!ncvisual_blit(gio->nc, resource->visual, &vopts)) {
            log_msg(ERROR, "media_output", "Failed to blit animation frame");
            resource->is_playing = false;
            return false;
        }

        // Make sure the plane is visible and render
        ncplane_move_top(resource->plane);
        notcurses_render(gio->nc);

        // Check for user input to allow interruption
        input_event_t input_event;
        if (get_input_nonblocking(&input_event)) {
            // Input detected - store it and interrupt animation
            if (interrupt_event) {
                *interrupt_event = input_event;
            }
            log_msg(DEBUG, "media_output", "Animation interrupted by user input");
            resource->is_playing = false;
            return true;// Return true to indicate successful completion (even though interrupted)
        }

        // Sleep for frame duration
#ifdef _WIN32
        Sleep(frame_delay_us / 1000);// Windows Sleep takes milliseconds
#else
        usleep(frame_delay_us);// Unix usleep takes microseconds
#endif

        // Try to decode next frame
        int decode_result = ncvisual_decode(resource->visual);
        if (decode_result == 1) {
            // End of animation reached
            if (loop) {
                // Reset to beginning for looping by recreating the visual
                log_msg(DEBUG, "media_output", "Animation loop completed, restarting");
                if (!recreate_visual_for_loop(resource)) {
                    log_msg(ERROR, "media_output", "Failed to recreate visual for looping");
                    resource->is_playing = false;
                    return false;
                }
                // Continue the loop with the newly recreated visual
                continue;
            } else {
                // Animation finished, not looping
                resource->is_playing = false;
                break;
            }
        } else if (decode_result < 0) {
            log_msg(ERROR, "media_output", "Error decoding animation frame");
            resource->is_playing = false;
            return false;
        }

        // Check if animation should continue playing
        // This allows external control via media_output_pause()
    } while (resource->is_playing);

    return true;
}

/**
 * @brief Helper function to recreate a visual from its file path for looping animations
 * 
 * @param resource The loaded visual resource to recreate
 * @return true on success, false on failure
 */
static bool recreate_visual_for_loop(loaded_visual_t* resource) {
    if (!resource || !resource->path) {
        log_msg(ERROR, "media_output", "Invalid resource for visual recreation");
        return false;
    }

    // Destroy the current visual if it exists
    if (resource->visual) {
        ncvisual_destroy(resource->visual);
        resource->visual = NULL;
    }

    // Recreate the visual from the file path
    resource->visual = ncvisual_from_file(resource->path);
    if (!resource->visual) {
        log_msg(ERROR, "media_output", "Failed to recreate visual from file: %s", resource->path);
        return false;
    }

    log_msg(DEBUG, "media_output", "Successfully recreated visual for looping: %s", resource->path);
    return true;
}