#include <assert.h>
#include <stdio.h>
#include "../src/game.h"
#include "test_database.h"

void test_add(){
    int x = 5;
    int y = 6;
    assert(add(x,y) == 11);
    printf("Test_add passed\n");
}

int main(void) {
    test_add();
    test_db_open();
    return 0;
}
