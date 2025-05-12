#ifndef MEDIA_OUTPUT_H
#define MEDIA_OUTPUT_H

#include <notcurses/notcurses.h>
#include <stdbool.h>

/**
 * @brief Structure to represent a loaded visual
 * 
 * This is an opaque structure that encapsulates a Notcurses visual.
 */
typedef struct loaded_visual_s loaded_visual_t;

// Scale types for display functions
#define SCALE_NONE 0     // No scaling, use original size
#define SCALE_PRESERVE 1 // Scale preserving aspect ratio
#define SCALE_STRETCH 2  // Stretch to exact dimensions
#define SCALE_CELL 3     // Scale to fit in a single cell

#define GOBLIN_IMAGE "goblin.png" // Example image file for goblin
#define ASTRONAUT_IMAGE "astronaut.gif" // Example GIF file for goblin

/**
 * @brief Initialize the media output handler
 * 
 * Must be called after the common output handler is initialized.
 * 
 * @return true on success, false on failure
 */
bool init_media_output();

/**
 * @brief Load an image file
 * 
 * Loads an image from the specified file path.
 * Supported formats depend on the Notcurses build (typically PNG, JPEG, WebP).
 * 
 * @param path Path to the image file
 * @param[out] width Optional pointer to store the image width
 * @param[out] height Optional pointer to store the image height
 * @return A handle to the loaded visual, or NULL on failure
 */
loaded_visual_t* load_image(const char* path, int* width, int* height);

/**
 * @brief Load an animated GIF
 * 
 * Loads an animated GIF from the specified file path.
 * 
 * @param path Path to the GIF file
 * @param[out] width Optional pointer to store the GIF width
 * @param[out] height Optional pointer to store the GIF height
 * @param[out] frames Optional pointer to store the number of frames
 * @return A handle to the loaded visual, or NULL on failure
 */
loaded_visual_t* load_gif(const char* path, int* width, int* height, int* frames);

/**
 * @brief Display a visual at the specified position with custom scaling
 * 
 * Renders a previously loaded visual at the given coordinates with custom scaling.
 * 
 * @param visual The visual to display
 * @param y The Y coordinate (row) for the top-left corner
 * @param x The X coordinate (column) for the top-left corner
 * @param scale_type How to scale the visual (SCALE_NONE, SCALE_PRESERVE, SCALE_STRETCH, SCALE_CELL)
 * @param target_width Target width for scaling (0 for auto/original size)
 * @param target_height Target height for scaling (0 for auto/original size)
 * @return true on success, false on failure
 */
bool display_visual(loaded_visual_t* visual, int y, int x, int scale_type,
                    int target_width, int target_height);

/**
 * @brief Display an image at fullscreen
 * 
 * Renders an image to fill the entire screen, stretching to fit.
 * 
 * @param visual The visual to display
 * @return true on success, false on failure
 */
bool display_image_fullscreen(loaded_visual_t* visual);

/**
 * @brief Display an image at the specified position and size
 * 
 * Renders an image at the specified position with the given dimensions.
 * 
 * @param visual The visual to display
 * @param y The Y coordinate (row) for the top-left corner
 * @param x The X coordinate (column) for the top-left corner
 * @param width The width to display the image (in terminal columns)
 * @param height The height to display the image (in terminal rows)
 * @return true on success, false on failure
 */
bool display_image_positioned(loaded_visual_t* visual, int y, int x, int width, int height);

/**
 * @brief Display an image scaled to fit a single cell
 * 
 * Renders an image scaled down to fit in a single terminal cell at the specified position.
 * Useful for map display when replacing characters with small images.
 * 
 * @param visual The visual to display
 * @param y The Y coordinate (row) for the cell
 * @param x The X coordinate (column) for the cell
 * @return true on success, false on failure
 */
bool display_image_cell(loaded_visual_t* visual, int y, int x);

/**
 * @brief Start playing an animated visual with custom scaling
 * 
 * Begins playback of an animated visual (like a GIF) with custom scaling options.
 * 
 * @param visual The visual to play
 * @param y The Y coordinate (row) for the top-left corner
 * @param x The X coordinate (column) for the top-left corner
 * @param scale_type How to scale the visual (SCALE_NONE, SCALE_PRESERVE, SCALE_STRETCH, SCALE_CELL)
 * @param target_width Target width for scaling (0 for auto/original size)
 * @param target_height Target height for scaling (0 for auto/original size)
 * @param loop Whether to loop the animation (true: loop, false: play once)
 * @return true on success, false on failure
 */
bool play_animated_visual(loaded_visual_t* visual, int y, int x, int scale_type,
                          int target_width, int target_height, bool loop);

/**
 * @brief Play a GIF at fullscreen
 * 
 * Plays an animated GIF to fill the entire screen, stretching to fit.
 * 
 * @param visual The visual to play
 * @param loop Whether to loop the animation (true: loop, false: play once)
 * @return true on success, false on failure
 */
bool play_gif_fullscreen(loaded_visual_t* visual, bool loop);

/**
 * @brief Play a GIF at the specified position and size
 * 
 * Plays an animated GIF at the specified position with the given dimensions.
 * 
 * @param visual The visual to play
 * @param y The Y coordinate (row) for the top-left corner
 * @param x The X coordinate (column) for the top-left corner
 * @param width The width to display the GIF (in terminal columns)
 * @param height The height to display the GIF (in terminal rows)
 * @param loop Whether to loop the animation (true: loop, false: play once)
 * @return true on success, false on failure
 */
bool play_gif_positioned(loaded_visual_t* visual, int y, int x, int width, int height, bool loop);

/**
 * @brief Play a GIF scaled to fit a single cell
 * 
 * Plays an animated GIF scaled down to fit in a single terminal cell at the specified position.
 * Useful for map display when replacing characters with small animated images.
 * 
 * @param visual The visual to play
 * @param y The Y coordinate (row) for the cell
 * @param x The X coordinate (column) for the cell
 * @param loop Whether to loop the animation (true: loop, false: play once)
 * @return true on success, false on failure
 */
bool play_gif_cell(loaded_visual_t* visual, int y, int x, bool loop);

/**
 * @brief Stop an animated visual
 * 
 * Stops playback of a previously started animated visual.
 * 
 * @param visual The visual to stop
 * @return true on success, false on failure
 */
bool stop_animated_visual(loaded_visual_t* visual);

// These functions are defined in common_output.h
// bool get_screen_dimensions(int* width, int* height);
// bool render_frame(void);

/**
 * @brief Free a loaded visual
 * 
 * Releases the resources associated with a loaded visual.
 * 
 * @param visual The visual to free
 */
void free_visual(loaded_visual_t* visual);

/**
 * @brief Shutdown the media output handler
 * 
 * Cleans up resources used by the media output handler.
 */
void shutdown_media_output(void);

#endif // MEDIA_OUTPUT_H