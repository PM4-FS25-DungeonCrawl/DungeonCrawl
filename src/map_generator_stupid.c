#include "map_generator_stupid.h"
#include "map_mode.h"

void generate_map() {
    enum map_tile map[WIDTH][HEIGHT];
    int playerX = 0;
    int playerY = 0;



    setNewMap(&map, playerX, playerY);
}
