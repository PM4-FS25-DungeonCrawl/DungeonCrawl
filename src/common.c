#include "common.h"

memory_pool_t* main_memory_pool;

/**
 * @brief Clears the entire screen by overwriting all characters with spaces.
 * This function can be reused across different modules to reset the screen.
 *
 * @param stdplane The standard ncplane to operate on.
 */
void clear_screen(struct ncplane* stdplane) {
    // Get the dimensions of the ncplane
    int width = ncplane_dim_x(stdplane);
    int height = ncplane_dim_y(stdplane);

    // Loop through each cell and overwrite it with a space
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            ncplane_printf_yx(stdplane, j, i, " ");
        }
    }
}
