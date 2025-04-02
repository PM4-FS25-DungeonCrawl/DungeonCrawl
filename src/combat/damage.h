#ifndef DAMAGE_H
#define DAMAGE_H


typedef enum {
    PHYSICAL,
    MAGICAL,
    DAMAGE_TYPE_COUNT
} damage_type_t;

typedef enum {
    D6 = 6,
    D8 = 8,
    D10 = 10,
    D12 = 12,
    D20 = 20
} dice_size_t;

typedef struct {
    damage_type_t type;
    int value;
} weakness_t;

weakness_t* init_weakness(damage_type_t type, int value);
void free_weakness(weakness_t* weaknesses);

#endif //DAMAGE_H
