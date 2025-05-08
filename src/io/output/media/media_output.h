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

/**
 * @brief Initialize the media output handler
 * 
 * Must be called after the common output handler is initialized.
 * 
 * @param nc The Notcurses instance to use
 * @return true on success, false on failure
 */
bool init_media_output(struct notcurses* nc);

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
 * @brief Display a visual at the specified position
 * 
 * Renders a previously loaded visual at the given coordinates.
 * 
 * @param visual The visual to display
 * @param y The Y coordinate (row) for the top-left corner
 * @param x The X coordinate (column) for the top-left corner
 * @param scale_type How to scale the visual (0: none, 1: scale, 2: stretch)
 * @param target_width Target width for scaling (0 for original size)
 * @param target_height Target height for scaling (0 for original size)
 * @return true on success, false on failure
 */
bool display_visual(loaded_visual_t* visual, int y, int x, int scale_type, 
                   int target_width, int target_height);

/**
 * @brief Start playing an animated visual
 * 
 * Begins playback of an animated visual (like a GIF).
 * 
 * @param visual The visual to play
 * @param y The Y coordinate (row) for the top-left corner
 * @param x The X coordinate (column) for the top-left corner
 * @param scale_type How to scale the visual (0: none, 1: scale, 2: stretch)
 * @param target_width Target width for scaling (0 for original size)
 * @param target_height Target height for scaling (0 for original size)
 * @param loop Whether to loop the animation (true: loop, false: play once)
 * @return true on success, false on failure
 */
bool play_animated_visual(loaded_visual_t* visual, int y, int x, int scale_type,
                         int target_width, int target_height, bool loop);

/**
 * @brief Stop an animated visual
 * 
 * Stops playback of a previously started animated visual.
 * 
 * @param visual The visual to stop
 * @return true on success, false on failure
 */
bool stop_animated_visual(loaded_visual_t* visual);

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