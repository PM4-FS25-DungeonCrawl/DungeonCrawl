//
// Created by jabar on 27.03.2025.
//
#include <stdio.h>
#include "../src/database/database.h"
#include <assert.h>


void test_db_open(){
    DBConnection db_connection;
    assert(db_open(&db_connection, "test.db") == 1);
    db_close(&db_connection);
    printf("Test_db_open passed\n");
}
