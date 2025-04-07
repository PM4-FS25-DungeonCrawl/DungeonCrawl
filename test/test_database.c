#include <stdio.h>
#include "../src/database/database.h"
#include "../src/database/localization/localization_database.h"
#include "../src/database/localization/attribute_database.h"
#include <assert.h>

#define TEST_ATTRIBUTE_NAME "RARESWORD"
#define EXPECTED_ATTRIBUTE_KEY 6

void test_db_open(){
    DBConnection db_connection;
    assert(db_open(&db_connection, "../../test/test_data.db") == 1);
    assert(db_is_open() == 1);
    db_close();
    assert(db_is_open() == 0);
    printf("Test_db_open passed\n");
}

void test_attribute_key() {
    DBConnection db_connection;
    assert(db_open(&db_connection, "../../test/test_data.db") == 1);
    assert(db_is_open() == 1);

    // Create a test attribute
    int attr_key = attribute_key(TEST_ATTRIBUTE_NAME);
    assert(attr_key != -1);

    // Check if the attribute key is correct
    assert(attr_key == EXPECTED_ATTRIBUTE_KEY);

    db_close();
    assert(db_is_open() == 0);
    printf("Test_attribute_key passed\n");
}


int main() {
    test_db_open();
    test_attribute_key();
    return 0;
}
