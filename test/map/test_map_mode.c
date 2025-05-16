#include "../src/character/character.h"
#include "../src/map/map.h"
#include "../src/map/map_mode.h"
#include "../src/memory/memory_management.h"

#include <assert.h>

memory_pool_t* test_map_mode_memory_pool;

void setup() {
    test_map_mode_memory_pool = init_memory_pool(MIN_MEMORY_POOL_SIZE);
    if (test_map_mode_memory_pool == NULL) {
        printf("Failed to initialize memory pool\n");
        exit(EXIT_FAILURE);
    }
    printf("Setup complete\n");
}

// initialize a character for testing
character_t* setup_character() {
    character_t* character = init_character(test_map_mode_memory_pool, PLAYER, "Hero");
    set_character_stats(character, 5, 10, 15, 20);
    return character;
}

void test_map_mode() {
    character_t* c = setup_character();

    // Setup keyevents
    input_event_t right = {
            .type = INPUT_RIGHT,
            .raw_input = {
                    .id = NCKEY_RIGHT}};

    input_event_t up = {
            .type = INPUT_UP,
            .raw_input = {
                    .id = NCKEY_UP}};

    input_event_t down = {
            .type = INPUT_DOWN,
            .raw_input = {
                    .id = NCKEY_DOWN}};

    input_event_t left = {
            .type = INPUT_LEFT,
            .raw_input = {
                    .id = NCKEY_LEFT}};

    // Initialize the map and player position
    init_map_mode();
    set_player_start_pos(1, 1);

    // Simulate player movement to an empty floor tile
    map[2][1] = FLOOR;

    map_mode_result_t result = handle_input(&right, c);
    vector2d_t player_pos = get_player_pos();
    assert(result == CONTINUE);
    assert(player_pos.dx == 2 && player_pos.dy == 1);

    // Simulate player encountering a goblin
    map[2][2] = GOBLIN;
    result = handle_input(&down, c);
    player_pos = get_player_pos();
    assert(result == COMBAT);
    assert(player_pos.dx == 2 && player_pos.dy == 2);

    // Simulate player hitting a wall
    map[1][2] = WALL;
    result = handle_input(&left, c);
    player_pos = get_player_pos();
    assert(result == CONTINUE);
    assert(player_pos.dx == 2 && player_pos.dy == 2);// Position should not change

    // Simulate player hitting start door
    map[3][2] = START_DOOR;
    result = handle_input(&right, c);
    player_pos = get_player_pos();
    assert(result == CONTINUE);
    assert(player_pos.dx == 2 && player_pos.dy == 2);// Position should not change

    // Simulate player hitting exit door
    map[2][3] = EXIT_DOOR;
    result = handle_input(&down, c);
    player_pos = get_player_pos();
    assert(result == CONTINUE);
    assert(player_pos.dx == 2 && player_pos.dy == 2);// Position should not change

    //Simulate player picking up a key
    map[2][1] = KEY;
    result = handle_input(&up, c);
    player_pos = get_player_pos();
    assert(result == CONTINUE);
    assert(player_pos.dx == 2 && player_pos.dy == 1);

    // Simulate player using the key on the exit door
    map[2][0] = EXIT_DOOR;
    result = handle_input(&up, c);
    player_pos = get_player_pos();
    assert(result == NEXT_FLOOR);
    assert(player_pos.dx == 2 && player_pos.dy == 0);

    printf("All map_mode tests passed.\n");
}

int main() {
    setup();

    test_map_mode();

    shutdown_memory_pool(test_map_mode_memory_pool);
    return 0;
}
