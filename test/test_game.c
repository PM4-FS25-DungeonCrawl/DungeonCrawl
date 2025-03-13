#include <assert.h>
#include <stdio.h>
#include "../include/termbox2.h"
#include "../src/game.h"

void test_add(){
    int x = 5;
    int y = 6;
    assert(add(x,y) == 11);
    printf("Test_add passed\n");
}

int main(void) {
    test_add();
    return 0;
}
