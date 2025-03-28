#include <assert.h>
#include <stdio.h>

#include "test_draw_light.h"

#include "../src/map/map.h"
#include "../src/map/drawop/draw_light.h"

constexpr int width = 10;
constexpr int height = 10;

enum map_tile test_map[width][height];
enum map_tile revealed_test_map[width][height];

void init_test_mazes(void) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            test_map[x][y] = FLOOR;
            revealed_test_map[x][y] = HIDDEN;
        }
    }
}

void reset_test_mazes(void) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            test_map[x][y] = FLOOR;
            revealed_test_map[x][y] = HIDDEN;
        }
    }
}

void print_array(const int *array) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            printf("%d ", array[x * height + y]);
        }
        printf("\n");
    }
    printf("\n");
}

void test_draw_light_on_player(void) {
    init_test_mazes();

    const Vector2D player = {5, 5};
    constexpr int light_radius = 3;
    draw_light_on_player((int *)test_map, (int *)revealed_test_map, height, width, player, light_radius);
    //print_array((int *)revealed_test_maze, height, width);
    //player position will not be drawn
    assert(revealed_test_map[5][5] == HIDDEN);
    revealed_test_map[5][5] = FLOOR;

    //test: correct light radius is drawn
    assert(revealed_test_map[2][5] == FLOOR);

    assert(revealed_test_map[3][4] == FLOOR);
    assert(revealed_test_map[3][5] == FLOOR);
    assert(revealed_test_map[3][6] == FLOOR);

    assert(revealed_test_map[4][3] == FLOOR);
    assert(revealed_test_map[4][4] == FLOOR);
    assert(revealed_test_map[4][5] == FLOOR);
    assert(revealed_test_map[4][6] == FLOOR);
    assert(revealed_test_map[4][7] == FLOOR);

    assert(revealed_test_map[5][2] == FLOOR);
    assert(revealed_test_map[5][3] == FLOOR);
    assert(revealed_test_map[5][4] == FLOOR);
    //skip player position
    assert(revealed_test_map[5][6] == FLOOR);
    assert(revealed_test_map[5][7] == FLOOR);
    assert(revealed_test_map[5][8] == FLOOR);

    assert(revealed_test_map[6][3] == FLOOR);
    assert(revealed_test_map[6][4] == FLOOR);
    assert(revealed_test_map[6][5] == FLOOR);
    assert(revealed_test_map[6][6] == FLOOR);
    assert(revealed_test_map[6][7] == FLOOR);

    assert(revealed_test_map[7][4] == FLOOR);
    assert(revealed_test_map[7][5] == FLOOR);
    assert(revealed_test_map[7][6] == FLOOR);

    assert(revealed_test_map[8][5] == FLOOR);
    printf("Test: \"correct light radius is drawn\" passed\n");
    //end test

    //new setup
    reset_test_mazes();
    test_map[4][4] = WALL;
    test_map[4][5] = WALL;
    test_map[4][6] = WALL;
    test_map[5][4] = WALL;
    test_map[5][6] = WALL;
    test_map[6][4] = WALL;
    test_map[6][5] = WALL;
    test_map[6][6] = WALL;

    draw_light_on_player((int *)test_map, (int *)revealed_test_map, height, width, player, light_radius);
    //print_array((int *)revealed_test_maze, height, width);
    //test: completely surrounded by walls, iter = 1
    assert(revealed_test_map[2][5] == HIDDEN);

    assert(revealed_test_map[3][4] == HIDDEN);
    assert(revealed_test_map[3][5] == HIDDEN);
    assert(revealed_test_map[3][6] == HIDDEN);

    assert(revealed_test_map[4][3] == HIDDEN);
    assert(revealed_test_map[4][4] == WALL);
    assert(revealed_test_map[4][5] == WALL);
    assert(revealed_test_map[4][6] == WALL);
    assert(revealed_test_map[4][7] == HIDDEN);

    assert(revealed_test_map[5][2] == HIDDEN);
    assert(revealed_test_map[5][3] == HIDDEN);
    assert(revealed_test_map[5][4] == WALL);
    //skip player position
    assert(revealed_test_map[5][6] == WALL);
    assert(revealed_test_map[5][7] == HIDDEN);
    assert(revealed_test_map[5][8] == HIDDEN);

    assert(revealed_test_map[6][3] == HIDDEN);
    assert(revealed_test_map[6][4] == WALL);
    assert(revealed_test_map[6][5] == WALL);
    assert(revealed_test_map[6][6] == WALL);
    assert(revealed_test_map[6][7] == HIDDEN);

    assert(revealed_test_map[7][4] == HIDDEN);
    assert(revealed_test_map[7][5] == HIDDEN);
    assert(revealed_test_map[7][6] == HIDDEN);

    assert(revealed_test_map[8][5] == HIDDEN);
    printf("Test: \"completely surrounded by walls (iter: 1)\" passed\n");

    draw_light_on_player((int *)test_map, (int *)revealed_test_map, height, width, player, light_radius);
    //print_array((int *)revealed_test_maze, height, width);
    //test: completely surrounded by walls, iter = 2
    assert(revealed_test_map[2][5] == HIDDEN);

    assert(revealed_test_map[3][4] == HIDDEN);
    assert(revealed_test_map[3][5] == HIDDEN);
    assert(revealed_test_map[3][6] == HIDDEN);

    assert(revealed_test_map[4][3] == HIDDEN);
    assert(revealed_test_map[4][4] == WALL);
    assert(revealed_test_map[4][5] == WALL);
    assert(revealed_test_map[4][6] == WALL);
    assert(revealed_test_map[4][7] == HIDDEN);

    assert(revealed_test_map[5][2] == HIDDEN);
    assert(revealed_test_map[5][3] == HIDDEN);
    assert(revealed_test_map[5][4] == WALL);
    //skip player position
    assert(revealed_test_map[5][6] == WALL);
    assert(revealed_test_map[5][7] == HIDDEN);
    assert(revealed_test_map[5][8] == HIDDEN);

    assert(revealed_test_map[6][3] == HIDDEN);
    assert(revealed_test_map[6][4] == WALL);
    assert(revealed_test_map[6][5] == WALL);
    assert(revealed_test_map[6][6] == WALL);
    assert(revealed_test_map[6][7] == HIDDEN);

    assert(revealed_test_map[7][4] == HIDDEN);
    assert(revealed_test_map[7][5] == HIDDEN);
    assert(revealed_test_map[7][6] == HIDDEN);

    assert(revealed_test_map[8][5] == HIDDEN);
    printf("Test: \"completely surrounded by walls (iter: 2)\" passed\n");
    //end test

    //new setup
    reset_test_mazes();
    for (int y = 0; y < height; y++) {
        test_map[y][4] = WALL;
        test_map[y][6] = WALL;
    }

    draw_light_on_player((int *)test_map, (int *)revealed_test_map, height, width, player, light_radius);
    //print_array((int *)revealed_test_maze, height, width);
    //test: hallway with walls top and bottom
    for (int i = 4; i <= 6; i++) {
        assert(revealed_test_map[i][3] == HIDDEN);
        assert(revealed_test_map[i][7] == HIDDEN);
    }
    for (int i = 3; i <= 7; i++) {
        assert(revealed_test_map[i][4] == WALL);
        assert(revealed_test_map[i][6] == WALL);
    }
    for (int i = 2; i <= 8; i++) {
        if (i != 5) {
            assert(revealed_test_map[i][5] == FLOOR);
        }
    }
    printf("Test: \"hallway with walls top and bottom\" passed\n");
    //end test

    //new setup
    reset_test_mazes();
    for (int x = 0; x < height; x++) {
        test_map[4][x] = WALL;
        test_map[6][x] = WALL;
    }

    draw_light_on_player((int *)test_map, (int *)revealed_test_map, height, width, player, light_radius);
    //test: hallway with walls left and right
    for (int i = 4; i <= 6; i++) {
        assert(revealed_test_map[3][i] == HIDDEN);
        assert(revealed_test_map[7][i] == HIDDEN);
    }
    for (int i = 3; i <= 7; i++) {
        assert(revealed_test_map[4][i] == WALL);
        assert(revealed_test_map[6][i] == WALL);
    }
    for (int i = 2; i <= 8; i++) {
        if (i != 5) {
            assert(revealed_test_map[5][i] == FLOOR);
        }
    }
    printf("Test: \"hallway with walls left and right\" passed\n");

    printf("test_draw_light_on_player all tests passed\n");
}
