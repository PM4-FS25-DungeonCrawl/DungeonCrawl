#include "../include/termbox2.h"
#include "../src/map/map.h"
#include "../src/map/map_mode.h"

#include <assert.h>

void test_map_mode() {
    // Initialize the map and player position
    init_map_mode();
    set_player_start_pos(1, 1);

    // Simulate player movement to an empty floor tile
    map[2][1] = FLOOR;
    map_mode_result_t result = handle_input(&(struct tb_event) {.key = TB_KEY_ARROW_RIGHT});
    vector2d_t player_pos = get_player_pos();
    assert(result == CONTINUE);
    assert(player_pos.dx == 2 && player_pos.dy == 1);

    // Simulate player encountering a goblin
    map[2][2] = GOBLIN;
    result = handle_input(&(struct tb_event) {.key = TB_KEY_ARROW_DOWN});
    player_pos = get_player_pos();
    assert(result == COMBAT);
    assert(player_pos.dx == 2 && player_pos.dy == 2);

    // Simulate player hitting a wall
    map[1][2] = WALL;
    result = handle_input(&(struct tb_event) {.key = TB_KEY_ARROW_LEFT});
    player_pos = get_player_pos();
    assert(result == CONTINUE);
    assert(player_pos.dx == 2 && player_pos.dy == 2);// Position should not change

    // Simulate player hitting start door
    map[3][2] = START_DOOR;
    result = handle_input(&(struct tb_event) {.key = TB_KEY_ARROW_RIGHT});
    player_pos = get_player_pos();
    assert(result == CONTINUE);
    assert(player_pos.dx == 2 && player_pos.dy == 2);// Position should not change

    // Simulate player hitting exit door
    map[2][3] = EXIT_DOOR;
    result = handle_input(&(struct tb_event) {.key = TB_KEY_ARROW_DOWN});
    player_pos = get_player_pos();
    assert(result == CONTINUE);
    assert(player_pos.dx == 2 && player_pos.dy == 2);// Position should not change

    //Simulate player picking up a key
    map[2][1] = KEY;
    result = handle_input(&(struct tb_event) {.key = TB_KEY_ARROW_UP});
    player_pos = get_player_pos();
    assert(result == CONTINUE);
    assert(player_pos.dx == 2 && player_pos.dy == 1);

    // Simulate player using the key on the exit door
    map[2][0] = EXIT_DOOR;
    result = handle_input(&(struct tb_event) {.key = TB_KEY_ARROW_UP});
    player_pos = get_player_pos();
    assert(result == NEXT_FLOOR);
    assert(player_pos.dx == 2 && player_pos.dy == 0);

    printf("All map_mode tests passed.\n");
}

int main() {
    test_map_mode();
    return 0;
}
