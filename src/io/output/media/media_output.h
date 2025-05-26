/**
 * @file media_output.h
 * @brief Exposes functions for drawing to the screen.
 */
#ifndef MEDIA_OUTPUT_H
#define MEDIA_OUTPUT_H

#include "media_output_handler.h"

#include <notcurses/notcurses.h>
#include <stdbool.h>

/* =========================================================================
 * IMAGE DISPLAY FUNCTIONS
 * ========================================================================= */

/**
 * Display an image file at specified coordinates with scaling
 * 
 * @param filename Image file name (PNG supported, loaded from src/art/png/)
 * @param x X coordinate in terminal cells
 * @param y Y coordinate in terminal cells
 * @param height Height in terminal cells
 * @param width Width in terminal cells (0 for auto)
 * @param scale_type Scaling mode to use
 * @return true on success, false on failure
 */
bool display_image_at(const char* filename, int x, int y, int height, int width, scale_type_t scale_type);

/**
 * Fill the background with an image file scaled to terminal size
 * 
 * @param filename Image file name (PNG supported, loaded from src/art/png/)
 * @return true on success, false on failure
 */
bool display_image_background(const char* filename);

/**
 * Fill a single terminal cell with an image
 * 
 * @param filename Image file name (PNG supported, loaded from src/art/png/)
 * @param x X coordinate in terminal cells
 * @param y Y coordinate in terminal cells
 * @return true on success, false on failure
 */
bool display_image_cell(const char* filename, int x, int y);

/* =========================================================================
 * ANIMATION DISPLAY FUNCTIONS
 * ========================================================================= */

/**
 * Display an animation file at specified coordinates with scaling
 * 
 * @param filename Animation file name (GIF supported, loaded from src/art/gif/)
 * @param x X coordinate in terminal cells
 * @param y Y coordinate in terminal cells
 * @param height Height in terminal cells
 * @param width Width in terminal cells (0 for auto)
 * @param scaling Scaling mode to use
 * @param fps Frames per second
 * @param loop Whether to loop the animation
 * @return true on success, false on failure
 */
bool display_gif_at(const char* filename, int x, int y, int height, int width, scale_type_t scale_type, float fps, bool loop);

/**
 * Fill the background with an animation file scaled to terminal size
 * 
 * @param filename Animation file name (GIF supported, loaded from src/art/gif/)
 * @param fps Frames per second
 * @param loop Whether to loop the animation
 * @return true on success, false on failure
 */
bool display_gif_background(const char* filename, float fps, bool loop);

/**
 * Display a video file at specified coordinates with scaling
 * 
 * @param filename Video file name (MP4 supported, loaded from src/art/mp4/)
 * @param x X coordinate in terminal cells
 * @param y Y coordinate in terminal cells
 * @param width Width in terminal cells
 * @param height Height in terminal cells
 * @param scale Scaling mode to use
 * @return true on success, false on failure
 */
bool display_video_at(const char* filename, int x, int y, int width, int height, scale_type_t scale);

/**
 * Fill the background with a video file scaled to terminal size
 * 
 * @param filename Video file name (MP4 supported, loaded from src/art/mp4/)
 * @param fps Frames per second
 * @param loop Whether to loop the video
 * @return true on success, false on failure
 */
bool display_video_background(const char* filename, float fps, bool loop);

/* =========================================================================
 * ANIMATION CONTROL FUNCTIONS
 * ========================================================================= */

/**
 * Start playback of an animation or video
 * Note: This does not handle actual timing, which must be done in the game loop
 * @param media Pointer to a loaded media instance
 */
void media_output_play(loaded_visual_t* media);

/**
 * Pause playback of an animation or video
 * @param media Pointer to a loaded media instance
 */
void media_output_pause(loaded_visual_t* media);

/**
 * Reset animation or video to the beginning
 * @param media Pointer to a loaded media instance
 */
void media_output_reset(loaded_visual_t* media);

#endif// MEDIA_OUTPUT_H
