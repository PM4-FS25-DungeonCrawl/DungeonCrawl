#ifndef COMBAT_MODE_H
#define COMBAT_MODE_H

typedef enum {
    PLAYER_WON,
    PLAYER_LOST,
    EXIT_GAME
} combat_result_t;

combat_result_t start_combat(player_t* player, monster_t* monster);

//=== functions needed for testing ===
bool roll_hit(const character_t* defender, const ability_t* ability);
int roll_damage(const ability_t* ability);
int deal_damage_to_monster(int damage, damage_type_t damage_type, monster_t* monster);
int deal_damage_to_player(int damage, damage_type_t damage_type, player_t* player);

#endif //COMBAT_MODE_H
