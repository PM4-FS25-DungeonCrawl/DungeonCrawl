#include <assert.h>
#include <stdio.h>

#include "test_map_mode.h"
#include "../src/map_mode.h"

int test_maze[HEIGHT][WIDTH];
int revealed_test_maze[HEIGHT][WIDTH];

void init_test_mazes(void) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            test_maze[y][x] = Floor;
            revealed_test_maze[y][x] = Hidden;
        }
    }
}

void reset_test_mazes(void) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            test_maze[y][x] = Floor;
            revealed_test_maze[y][x] = Hidden;
        }
    }
}

void print_array(const int *array, const int height, const int width) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            printf("%d ", array[y * width + x]);
        }
        printf("\n");
    }
    printf("\n");
}

void test_draw_light_on_player(void) {
    init_test_mazes();

    const Vector2D player = {5, 5};
    constexpr int light_radius = 3;
    draw_light_on_player((int *)test_maze, (int *)revealed_test_maze, HEIGHT, WIDTH, player, light_radius);
    //print_array((int *)revealed_test_maze, HEIGHT, WIDTH);
    //player position will not be drawn
    assert(revealed_test_maze[5][5] == Hidden);
    revealed_test_maze[5][5] = Floor;

    //test: correct light radius is drawn
    assert(revealed_test_maze[2][5] == Floor);

    assert(revealed_test_maze[3][4] == Floor);
    assert(revealed_test_maze[3][5] == Floor);
    assert(revealed_test_maze[3][6] == Floor);

    assert(revealed_test_maze[4][3] == Floor);
    assert(revealed_test_maze[4][4] == Floor);
    assert(revealed_test_maze[4][5] == Floor);
    assert(revealed_test_maze[4][6] == Floor);
    assert(revealed_test_maze[4][7] == Floor);

    assert(revealed_test_maze[5][2] == Floor);
    assert(revealed_test_maze[5][3] == Floor);
    assert(revealed_test_maze[5][4] == Floor);
    //skip player position
    assert(revealed_test_maze[5][6] == Floor);
    assert(revealed_test_maze[5][7] == Floor);
    assert(revealed_test_maze[5][8] == Floor);

    assert(revealed_test_maze[6][3] == Floor);
    assert(revealed_test_maze[6][4] == Floor);
    assert(revealed_test_maze[6][5] == Floor);
    assert(revealed_test_maze[6][6] == Floor);
    assert(revealed_test_maze[6][7] == Floor);

    assert(revealed_test_maze[7][4] == Floor);
    assert(revealed_test_maze[7][5] == Floor);
    assert(revealed_test_maze[7][6] == Floor);

    assert(revealed_test_maze[8][5] == Floor);
    printf("Test: \"correct light radius is drawn\" passed\n");
    //end test

    //new setup
    reset_test_mazes();
    test_maze[4][4] = Wall;
    test_maze[4][5] = Wall;
    test_maze[4][6] = Wall;
    test_maze[5][4] = Wall;
    test_maze[5][6] = Wall;
    test_maze[6][4] = Wall;
    test_maze[6][5] = Wall;
    test_maze[6][6] = Wall;

    draw_light_on_player((int *)test_maze, (int *)revealed_test_maze, HEIGHT, WIDTH, player, light_radius);
    //print_array((int *)revealed_test_maze, HEIGHT, WIDTH);
    //test: completely surrounded by walls, iter = 1
    assert(revealed_test_maze[2][5] == Hidden);

    assert(revealed_test_maze[3][4] == Hidden);
    assert(revealed_test_maze[3][5] == Hidden);
    assert(revealed_test_maze[3][6] == Hidden);

    assert(revealed_test_maze[4][3] == Hidden);
    assert(revealed_test_maze[4][4] == Wall);
    assert(revealed_test_maze[4][5] == Wall);
    assert(revealed_test_maze[4][6] == Wall);
    assert(revealed_test_maze[4][7] == Hidden);

    assert(revealed_test_maze[5][2] == Hidden);
    assert(revealed_test_maze[5][3] == Hidden);
    assert(revealed_test_maze[5][4] == Wall);
    //skip player position
    assert(revealed_test_maze[5][6] == Wall);
    assert(revealed_test_maze[5][7] == Hidden);
    assert(revealed_test_maze[5][8] == Hidden);

    assert(revealed_test_maze[6][3] == Hidden);
    assert(revealed_test_maze[6][4] == Wall);
    assert(revealed_test_maze[6][5] == Wall);
    assert(revealed_test_maze[6][6] == Wall);
    assert(revealed_test_maze[6][7] == Hidden);

    assert(revealed_test_maze[7][4] == Hidden);
    assert(revealed_test_maze[7][5] == Hidden);
    assert(revealed_test_maze[7][6] == Hidden);

    assert(revealed_test_maze[8][5] == Hidden);
    printf("Test: \"completely surrounded by walls (iter: 1)\" passed\n");

    draw_light_on_player((int *)test_maze, (int *)revealed_test_maze, HEIGHT, WIDTH, player, light_radius);
    //print_array((int *)revealed_test_maze, HEIGHT, WIDTH);
    //test: completely surrounded by walls, iter = 2
    assert(revealed_test_maze[2][5] == Hidden);

    assert(revealed_test_maze[3][4] == Hidden);
    assert(revealed_test_maze[3][5] == Hidden);
    assert(revealed_test_maze[3][6] == Hidden);

    assert(revealed_test_maze[4][3] == Hidden);
    assert(revealed_test_maze[4][4] == Wall);
    assert(revealed_test_maze[4][5] == Wall);
    assert(revealed_test_maze[4][6] == Wall);
    assert(revealed_test_maze[4][7] == Hidden);

    assert(revealed_test_maze[5][2] == Hidden);
    assert(revealed_test_maze[5][3] == Hidden);
    assert(revealed_test_maze[5][4] == Wall);
    //skip player position
    assert(revealed_test_maze[5][6] == Wall);
    assert(revealed_test_maze[5][7] == Hidden);
    assert(revealed_test_maze[5][8] == Hidden);

    assert(revealed_test_maze[6][3] == Hidden);
    assert(revealed_test_maze[6][4] == Wall);
    assert(revealed_test_maze[6][5] == Wall);
    assert(revealed_test_maze[6][6] == Wall);
    assert(revealed_test_maze[6][7] == Hidden);

    assert(revealed_test_maze[7][4] == Hidden);
    assert(revealed_test_maze[7][5] == Hidden);
    assert(revealed_test_maze[7][6] == Hidden);

    assert(revealed_test_maze[8][5] == Hidden);
    printf("Test: \"completely surrounded by walls (iter: 2)\" passed\n");
    //end test

    //new setup
    reset_test_mazes();
    for (int y = 0; y < HEIGHT; y++) {
        test_maze[y][4] = Wall;
        test_maze[y][6] = Wall;
    }

    draw_light_on_player((int *)test_maze, (int *)revealed_test_maze, HEIGHT, WIDTH, player, light_radius);
    //print_array((int *)revealed_test_maze, HEIGHT, WIDTH);
    //test: hallway with walls left and right
    for (int i = 4; i <= 6; i++) {
        assert(revealed_test_maze[i][3] == Hidden);
        assert(revealed_test_maze[i][7] == Hidden);
    }
    for (int i = 3; i <= 7; i++) {
        assert(revealed_test_maze[i][4] == Wall);
        assert(revealed_test_maze[i][6] == Wall);
    }
    for (int i = 2; i <= 8; i++) {
        if (i != 5) {
            assert(revealed_test_maze[i][5] == Floor);
        }
    }
    printf("Test: \"hallway with walls left and right\" passed\n");
    //end test

    //new setup
    reset_test_mazes();
    for (int x = 0; x < HEIGHT; x++) {
        test_maze[4][x] = Wall;
        test_maze[6][x] = Wall;
    }

    draw_light_on_player((int *)test_maze, (int *)revealed_test_maze, HEIGHT, WIDTH, player, light_radius);
    //test: hallway with walls top and bottom
    for (int i = 4; i <= 6; i++) {
        assert(revealed_test_maze[3][i] == Hidden);
        assert(revealed_test_maze[7][i] == Hidden);
    }
    for (int i = 3; i <= 7; i++) {
        assert(revealed_test_maze[4][i] == Wall);
        assert(revealed_test_maze[6][i] == Wall);
    }
    for (int i = 2; i <= 8; i++) {
        if (i != 5) {
            assert(revealed_test_maze[5][i] == Floor);
        }
    }
    printf("Test: \"hallway with walls top and bottom\" passed\n");

    printf("Test_draw_light_on_player all tests passed\n");
}
