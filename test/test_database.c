#include "../src/database/database.h"
#include "../src/database/local/attribute_database.h"
#include "../src/database/local/local_database.h"

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

void test_db_open() {
    db_connection_t db_connection;
    assert(db_open(&db_connection, "../test/test_data.db") == 0);
    assert(db_is_open(&db_connection) == 1);

    db_close(&db_connection);
    assert(db_is_open(&db_connection) == 0);
    printf("Test_db_open passed\n");
}

void test_attribute_key() {
    assert(db_open(&db_connection, "../test/test_data.db") == 0);
    assert(db_is_open(&db_connection) == 1);

    // Create a test attribute
    const int attr_key = attribute_key(&db_connection, TEST_ATTRIBUTE_NAME);
    assert(attr_key != -1);

    // Check if the attribute key is correct
    assert(attr_key == EXPECTED_ATTRIBUTE_KEY);

    db_close(&db_connection);
    assert(db_is_open(&db_connection) == 0);
    printf("Test_attribute_key passed\n");
}

void test_localization_string() {
    assert(db_open(&db_connection, "../test/test_data.db") == 0);
    assert(db_is_open(&db_connection) == 1);

    // Create a test localization string
    const local_language_t LING_EN = LANGUAGE_EN;
    const char* localization_string = get_localization_string(&db_connection,
                                                              TEST_ATTRIBUTE_PATH, &LING_EN);
    assert(localization_string != NULL);

    // Check if the localization string is correct
    assert(strcmp(localization_string, EXPECTED_LOCALIZATION_STRING_EN) == 0);

    // Create a test localization string in German
    local_language_t LING_DE = LANGUAGE_DE;
    const char* localization_string_de = get_localization_string(&db_connection,
                                                                 TEST_ATTRIBUTE_PATH, &LING_DE);
    assert(localization_string_de != NULL);

    // Check if the localization string is correct
    assert(strcmp(localization_string_de, EXPECTED_LOCALIZATION_STRING_DE) == 0);

    db_close(&db_connection);
    assert(db_is_open(&db_connection) == 0);
    printf("Test_localization_string passed\n");
}


int main() {
    test_db_open();
    test_attribute_key();
    test_localization_string();
    return 0;
}
