# needed files for each test
helper_db = files(
    # used to resolve compile errors with log_msg
    '../src/logging/logger.c',
    '../src/logging/ringbuffer.c',
    # actual needed files
    '../include/sqlite3.c',
    '../src/database/database.c',
    '../src/database/local/local_database.c',

    '../src/logging/logger.c',
    '../src/logging/ringbuffer.c',
)

helper_draw_light = files(
    '../src/map/map.c',
    '../src/map/draw/draw_light.c',

    '../src/logging/logger.c',
    '../src/logging/ringbuffer.c',
)

helper_combat = files(
    '../src/combat/combat_mode.c',
    '../src/combat/ability.c',
    '../src/combat/damage.c',
    '../src/combat/draw/draw_combat_mode.c',

    '../src/character/character.c',
    '../src/character/level.c',
    '../src/character/player.c',
    '../src/character/monster.c',

    '../src/item/gear.c',
    '../src/item/potion.c',

    '../include/termbox2.c',
    '../src/logging/logger.c',
    '../src/logging/ringbuffer.c',

    '../src/local/local.c',
    '../include/sqlite3.c',
    '../src/database/database.c',
    '../src/database/local/local_database.c'
)


# needed directories for each test
#incdir_database = include_directories(
#    '../test/'
#)

helper_ringbuffer = files(
    '../src/logging/ringbuffer.c'
)

# executables
helper_memory = files(
    '../src/logging/logger.c',
    '../src/logging/ringbuffer.c',
    '../src/memory/memory_management.c'
)

test_combat_mode = executable('test_combat_mode', 'combat/test_combat_mode.c', helper_combat, c_args: ['-w'])
test_character = executable('test_character', 'character/test_character.c', helper_combat, c_args: ['-w'])
test_database = executable('test_database', 'test_database.c', helper_db, c_args: ['-w'])
test_draw_light = executable('test_draw_light', 'map/draw/test_draw_light.c', helper_draw_light, c_args: ['-w'])
test_damage = executable('test_damage', 'combat/test_damage.c', helper_combat, c_args: ['-w'])
test_ringbuffer = executable('test_ringbuffer', 'logging/test_ringbuffer.c', helper_ringbuffer, c_args: ['-w'])
test_memory_management = executable('test_memory_management', 'memory/test_memory_management.c', helper_memory, c_args: ['-w'])


# test
test('test_combat_mode', test_combat_mode)
test('test_character', test_character)
test('test_database', test_database)
test('test_draw_light', test_draw_light)
test('test_damage', test_damage)
test('test_ringbuffer', test_ringbuffer)
test('test_memory_management', test_memory_management)
