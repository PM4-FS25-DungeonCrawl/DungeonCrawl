#ifndef MAP_MODE_H
#define MAP_MODE_H

//#define DEBUG_TRUE

#define COLOR_FOREGROUND TB_WHITE
#define COLOR_BACKGROUND TB_BLACK


#define CONTINUE 0
#define QUIT 1

#ifdef TEST_MAP_MODE
    //test mode is active
    #define DEBUG_PRINT(x, y, ...) printf(__VA_ARGS__)
#else
    #ifdef DEBUG_TRUE
        #define DEBUG_PRINT(x, y, ...) print_text(x, y, __VA_ARGS__)
    #else
        //empty debug print
        #define DEBUG_PRINT(x, y, ...)
    #endif

    #define LIGHT_RADIUS 3
    #define WIDTH 20
    #define HEIGHT 10
    extern int maze[HEIGHT][WIDTH];
#endif

/**
 * @brief 2-dimensional vector struct
 */
typedef struct {
    int x, y;
} Vector2D;

/**
 * @brief Enum for the different tiles in the maze
 */
enum tiles {
    Floor = 0,
    Wall = 1,
    Hidden = 2
};

/**
 * Draws the light around the player on the revealed_map.
 * The algorithmen reveals the map around the player, based on the base_map, in the revealed_map.
 * The light radius is the distance from the player, where the map tile is still visible.
 * A light radius of 3 means 3 tiles are visible in each direction (up, down, left, right) from the player.
 *
 * @param base_map the pointer to the 2D array containing all the map tiles (no Hidden tiles)
 * @param revealed_map the pointer to the 2D array to reveal the base_map,
 * based on the player's position and light radius
 * @param height the height of the map
 * @param width the width of the map
 * @param player the player's position on the map
 * @param light_radius the radius of the light around the player
 */
void draw_light_on_player(const int* base_map, int* revealed_map, int height, int width,
                          Vector2D player, int light_radius);

void set_start(int newPlayerX, int newPlayerY);


/**
 * Updates the player position based on the player's input and redraws the maze.
 * @return CONTINUE (0) if the game continue, QUIT (1) if the player pressed the exit key.
 */
int map_mode_update(void);


int init_map_mode(void);

#endif //MAP_MODE_H
