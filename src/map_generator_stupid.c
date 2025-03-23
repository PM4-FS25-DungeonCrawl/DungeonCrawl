#include <stdlib.h>
#include <time.h>
#include "map_generator_stupid.h"
#include "map_mode.h"

#define LOOP_COUNT 26
#define MAX_LINES 100


typedef struct {
    int x1, y1, x2, y2;
} LineSegment;
LineSegment drawnLines[MAX_LINES];
int lineCount = 0;

enum map_tile newMap[WIDTH][HEIGHT];

void initialize_random_seed() {
    unsigned int seed = time(nullptr);
    srand(seed);
}

void createRandomCoordinates(int *x, int *y) {
    *x = rand() % WIDTH;
    *y = rand() % HEIGHT;
}

bool isAdjacentOrParallel(LineSegment newLine) {
    for (int i = 0; i < lineCount; i++) {
        LineSegment existingLine = drawnLines[i];
        // Check if the new line is parallel and adjacent to the existing line
        if ((newLine.x1 == newLine.x2 && existingLine.x1 == existingLine.x2 && abs(newLine.x1 - existingLine.x1) <= 1) ||
            (newLine.y1 == newLine.y2 && existingLine.y1 == existingLine.y2 && abs(newLine.y1 - existingLine.y1) <= 1)) {
            return true;
            }
    }
    return false;
}

void drawLine(int x1, int y1, int x2, int y2) {
    LineSegment newLine = {x1, y1, x2, y2};
    if (isAdjacentOrParallel(newLine)) {
        return; // Skip drawing this line
    }

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    int e2;

    while (true) {
        newMap[x1][y1] = FLOOR;
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }

    // Record the drawn line
    if (lineCount < MAX_LINES) {
        drawnLines[lineCount++] = newLine;
    }
}

void drawPath(int x1, int y1, int x2, int y2) {
    int direction = rand() % 2;

    if (direction == 0) {
        drawLine(x1, y1, x2, y1);
        drawLine(x2, y1, x2, y2);
    } else {
        drawLine(x1, y1, x1, y2);
        drawLine(x1, y2, x2, y2);
    }
}

void generate_map() {
    initialize_random_seed();

    int playerX = 0;
    int playerY = 0;

    // Initialize all cells as walls
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            newMap[x][y] = WALL;
        }
    }

    int x1, y1, x2, y2;
    createRandomCoordinates(&x1, &y1);
    createRandomCoordinates(&x2, &y2);

    for (int i = 0; i < LOOP_COUNT; i++) {
        createRandomCoordinates(&x1, &y1);
        createRandomCoordinates(&x2, &y2);
        drawPath(x1, y1, x2, y2);
    }

    // Place the player at a random valid position
    do {
        createRandomCoordinates(&playerX, &playerY);
    } while (newMap[playerX][playerY] != FLOOR);

    setNewMap(&newMap, playerX, playerY);
}
