#ifndef MAIN_H
#define MAIN_H

#include "memory/memory_management.h"

typedef enum {
    SUCCESS,
    FAIL_TB_INIT,
    FAIL_MEM_POOL_INIT,
    FAIL_LOCAL_INIT,
    FAIL_GAME_MODE_INIT,
    FAIL_GAME_ENTITY_INIT,
    FAIL_ERROR,
} exit_code_t;

int main(void);

extern memory_pool_t* main_memory_pool;

#endif// MAIN_H
