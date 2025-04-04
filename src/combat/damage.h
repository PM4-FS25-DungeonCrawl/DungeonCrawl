#ifndef DAMAGE_H
#define DAMAGE_H

#define DAMAGE_TYPE_COUNT 2

typedef enum {
    PHYSICAL,
    MAGICAL
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
} damage_modifier_t;

#endif //DAMAGE_H
