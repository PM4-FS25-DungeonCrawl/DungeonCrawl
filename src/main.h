#ifndef MAIN_H
#define MAIN_H

extern volatile int init_done;

typedef enum {
    SUCCESS,
    FAIL_TB_INIT,
    FAIL_MEM_POOL_INIT,
    FAIL_LOCAL_INIT,
    FAIL_IO_HANDLER_INIT,
    FAIL_MAIN_MENU_INIT,
    FAIL_GAME_MODE_INIT,
    FAIL_INVENTORY_MODE_INIT,
    FAIL_GAME_ENTITY_INIT,
    FAIL_STATS_MODE_INIT,
    FAIL_ERROR,
} exit_code_t;

int main(void);

#endif// MAIN_H
