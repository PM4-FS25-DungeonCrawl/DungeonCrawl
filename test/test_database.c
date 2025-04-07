#include "../src/database/database.h"
#include <assert.h>
#include <stdio.h>


void test_db_open() {
    DBConnection db_connection;
    assert(db_open(&db_connection, "test.db") == 1);
    db_close(&db_connection);
    printf("Test_db_open passed\n");
}


int main() {
    test_db_open();
    return 0;
}
