#include "../src/database/database.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

// Macros for test attribute key
#define TEST_ATTRIBUTE_NAME "RARESWORD"
#define EXPECTED_ATTRIBUTE_KEY 6

// Macros for test localization string
#define TEST_ATTRIBUTE_PATH "ITEM.RARESWORD.NAME.TEXT"
#define EXPECTED_LOCALIZATION_STRING_EN "Rare Sword"
#define EXPECTED_LOCALIZATION_STRING_DE "Seltenes Schwert"

db_connection_t db_connection;

void test_db_open() {
    assert(db_open(&db_connection, "../test/database/test_data.db") == DB_OPEN_STATUS_SUCCESS);
    assert(db_is_open(&db_connection) == 1);

    db_close(&db_connection);
    assert(db_is_open(&db_connection) == 0);
    printf("Test_db_open passed\n");
}

void test_attribute_key() {
    assert(db_open(&db_connection, "../test/database/test_data.db") == DB_OPEN_STATUS_SUCCESS);
    assert(db_is_open(&db_connection) == 1);

    db_close(&db_connection);
    assert(db_is_open(&db_connection) == 0);
    printf("Test_attribute_key passed\n");
}


int main() {
    test_db_open();
    test_attribute_key();
    return 0;
}
