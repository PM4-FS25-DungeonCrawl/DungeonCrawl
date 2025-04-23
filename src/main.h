#ifndef MAIN_H
#define MAIN_H

typedef enum {
    SUCCESS = 0,
    FAIL_TB_INIT = 1,
    FAIL_LOCAL_INIT = 2,
    FAIL_GAME_ENTITY_INIT = 3,
    FAIL_ERROR = 4,
} exit_code_t;

int main(void);

#endif// MAIN_H