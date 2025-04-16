#ifndef LOCAL_DATABASE_H
#define LOCAL_DATABASE_H
#include "../../local/local.h"
#include "../database.h"

// functions for localization table
char* get_localization_string(const db_connection_t* db_connection, const char* attribute_path, const local_language_t* language);
#endif//LOCAL_DATABASE_H
