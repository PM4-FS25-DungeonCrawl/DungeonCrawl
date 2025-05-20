#include "media_output.h"

#include "../../../logging/logger.h"
#include "../../io_handler.h"        // Include this to access global nc and stdplane
#include "../common/output_handler.h"// For get_screen_dimensions and render_frame

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "media_output_handler.h"
#include "media_files.h"


/* =========================================================================
 * FORWARD DECLARATIONS
 * ========================================================================= */

// Forward declarations for internal functions
static bool display_image(loaded_visual_t *resource);

//static bool display_animation(loaded_visual_t *resource, float fps, bool loop);

/* =========================================================================
 * PNG DISPLAY FUNCTIONS
 * ========================================================================= */

// Display a PNG file at specified coordinates with scaling
bool display_image_at(const char *filename, int x, int y, int height, int width, scale_type_t scale_type) {
    // Validate parameters
    if (!filename || height < 0) {
        // Allow height=0 for automatic scaling
        log_msg(ERROR, "media_output", "Invalid parameters for display_png_at");
        return false;
    }
    loaded_visual_t *resource = ready_media(filename, x, y, height, width, scale_type);
    if (!resource) {
        log_msg(ERROR, "media_output", "Failed to load image for display");
        return false;
    }
    return display_image(resource);
}

// Fill the background with a PNG file scaled to terminal size
/*bool display_image_background(const char* filename) {
    // coming soon...
    return false;
}*/

// Fill a single terminal cell with a PNG image
bool display_image_cell(const char *filename, int x, int y) {
    // Validate parameters
    if (!filename) {
        log_msg(ERROR, "media_output", "Invalid filename for fill_cell_with_png");
        return false;
    }
    loaded_visual_t *resource = ready_media(filename, x, y, 1, 1, SCALE_CELL);
    if (!resource) {
        log_msg(ERROR, "media_output", "Failed to load image for cell");
        return false;
    }
    return display_image(resource);
}

/* =========================================================================
 * ANIMATION DISPLAY FUNCTIONS
 * ========================================================================= */

// Display a GIF file at specified coordinates with scaling
/*bool display_gif_at(const char* filename, int x, int y, int height, int width, scale_type_t scale_type, float fps, bool loop) {
    // coming soon
    return false;
}*/

// Fill the background with a GIF file scaled to terminal size
/*bool display_gif_background(const char* filename, float fps, bool loop) {
    // comign soon
    return false;
}*/

/*bool display_video_at(const char* filename, int x, int y, int width, int height, scale_type_t scale) {
    // coming soon...
    return false;
}*/

/*bool display_video_background(const char* filename, float fps, bool loop) {
    // coming soon...
    return false;
}*/

/* =========================================================================
 * ANIMATION CONTROL FUNCTIONS
 * ========================================================================= */

/*void media_output_play(loaded_visual_t* media) {
    //coming soon...
}*/

/*void media_output_pause(loaded_visual_t* media) {
    //coming soon...
}*/

/*void media_output_reset(loaded_visual_t* media) {
    //coming soon...
}*/

/* =========================================================================
* INTERNAL DISPLAY FUNCTIONS
* ========================================================================= */

// Helper function for displaying images
static bool display_image(loaded_visual_t *resource) {
    // Validate parameters
    if (!resource || !resource->visual) {
        log_msg(ERROR, "media_output", "Invalid parameters for display_image");
        return false;
    }

    if (resource->media_type == MEDIA_GIF) {
        // If it's a GIF, display it as an animation with default 10 FPS
        return false; //TODO: return display_animation(resource, 10.0f, false);
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
    opts.rows = resource->options.leny > 0 ? resource->options.leny : resource->og_height;
    opts.cols = resource->options.lenx > 0 ? resource->options.lenx : resource->og_width;

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
    vopts.y = 0; // Relative to the plane
    vopts.x = 0; // Relative to the plane
    vopts.scaling = resource->options.scaling;
    vopts.blitter = NCBLIT_2x2; // Simple blitter that works better

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
    notcurses_render(nc); // Directly call notcurses_render for maximum compatibility

    log_msg(INFO, "media_output", "Successfully displayed image");
    return true;
}

// Helper function for displaying animations
/*static bool display_animation(loaded_visual_t *resource, float fps, bool loop) {
    // coming soon
    return false;
}*/
