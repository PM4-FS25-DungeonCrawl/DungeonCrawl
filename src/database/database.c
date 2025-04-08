#include "database.h"

#include <stdio.h>

DBConnection* dungeon_crawl_db = NULL;

int db_open(DBConnection* db_connection, const char* db_name) {
    dungeon_crawl_db = db_connection;
    int rc = sqlite3_open(db_name, &dungeon_crawl_db->db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(dungeon_crawl_db->db));
        return 0;
    }
    return 1;
}

void db_close() {
    sqlite3_close(dungeon_crawl_db->db);
    dungeon_crawl_db->db = NULL;
}

int db_is_open() {
    return dungeon_crawl_db->db != NULL;
}

DBConnection* db_get_connection() {
    return dungeon_crawl_db;
}
