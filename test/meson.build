# needed files for each test
helper_db = files(
    # used to resolve compile errors with log_msg
    '../src/logging/logger.c',
    '../src/logging/ringbuffer.c',
    # actual needed files
    '../include/sqlite3.c',
    '../src/database/database.c',
    '../src/database/local/local_database.c',
    '../src/database/gamestate/gamestate_database.c',

    '../src/logging/logger.c',
    '../src/logging/ringbuffer.c',
    '../src/thread/thread_handler.c',
)

helper_draw_light = files(
    '../src/map/map.c',
    '../src/map/draw/draw_light.c',

    '../src/logging/logger.c',
    '../src/logging/ringbuffer.c',
    '../src/thread/thread_handler.c',
)

helper_combat = files(
    '../src/common.c',

    '../src/combat/combat_mode.c',
    '../src/combat/ability.c',
    '../src/combat/damage.c',

    '../src/combat/draw/draw_combat_mode.c',

    '../src/character/character.c',
    '../src/character/level.c',
    '../src/character/player.c',
    '../src/character/monster.c',

    '../src/inventory/inventory_mode.c',
    '../src/inventory/draw/draw_inventory_mode.c',

    '../src/item/gear.c',
    '../src/item/potion.c',

    '../src/logging/logger.c',
    '../src/logging/ringbuffer.c',
    '../src/thread/thread_handler.c',

    '../src/local/local.c',
    '../include/sqlite3.c',
    '../src/database/database.c',
    '../src/database/local/local_database.c',

    'test_helper.c',

    '../src/memory/memory_management.c'
)

helper_map_generator = files(
    '../src/map/map.c',
    '../src/map/map_generator.c',
    '../src/map/map_populator.c',
    '../src/map/map_mode.c',
    '../src/map/draw/draw_map_mode.c',
    '../src/map/draw/draw_light.c',

    '../src/logging/logger.c',
    '../src/logging/ringbuffer.c',
    '../src/thread/thread_handler.c',
)

helper_map_mode = files(
    '../src/map/map.c',
    '../src/map/map_mode.c',
    '../src/map/draw/draw_map_mode.c',
    '../src/map/draw/draw_light.c',

    '../src/logging/logger.c',
    '../src/logging/ringbuffer.c',
    '../src/thread/thread_handler.c',
)


# needed directories for each test
#incdir_database = include_directories(
#    '../test/'
#)

helper_ringbuffer = files(
    '../src/logging/ringbuffer.c'
)

incdir_common = files(
    '../src/logging/logger.h',
)

# executables
helper_memory = files(
    '../src/logging/logger.c',
    '../src/logging/ringbuffer.c',
    '../src/thread/thread_handler.c',
    '../src/memory/memory_management.c'
)

helper_stats = files(
    '../src/common.c',

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

    '../src/logging/logger.c',
    '../src/logging/ringbuffer.c',
    '../src/thread/thread_handler.c',

    '../src/local/local.c',
    '../include/sqlite3.c',
    '../src/database/database.c',
    '../src/database/local/local_database.c',

    'test_helper.c',

    '../src/memory/memory_management.c',


    '../src/stats/stats.c',
    '../src/stats/draw/draw_stats.c',
    '../src/stats/stats_mode.c'
)

notcurses = dependency('notcurses')

#test_combat_mode = executable('test_combat_mode', 'combat/test_combat_mode.c', helper_combat, c_args: ['-w'],dependencies: notcurses)
test_character = executable('test_character', 'character/test_character.c', helper_combat, c_args: ['-w'],dependencies: notcurses)
test_database = executable('test_database', 'database/test_database.c', helper_db, c_args: ['-w'],dependencies: notcurses)
test_draw_light = executable('test_draw_light', 'map/draw/test_draw_light.c', helper_draw_light, c_args: ['-w'],dependencies: notcurses)
test_damage = executable('test_damage', 'combat/test_damage.c', helper_combat, c_args: ['-w'],dependencies: notcurses)
test_ringbuffer = executable('test_ringbuffer', 'logging/test_ringbuffer.c', helper_ringbuffer, c_args: ['-w'],dependencies: notcurses)
test_memory_management = executable('test_memory_management', 'memory/test_memory_management.c', helper_memory, c_args: ['-w'],dependencies: notcurses)
#test_gamestate_database = executable('test_gamestate_database', 'database/test_gamestate_database.c', helper_db, c_args : ['-w'],dependencies: notcurses)
#test_map_generator = executable('test_map_generator', 'map/test_map_generator.c', helper_map_generator, c_args : ['-w'],dependencies: notcurses)
#test_map_mode = executable('test_map_mode', 'map/test_map_mode.c', helper_map_mode, c_args : ['-w'],dependencies: notcurses)
test_stats = executable('test_stats', 'stats/test_stats.c', helper_stats, c_args: ['-w'],dependencies: notcurses)


# test
#test('test_combat_mode', test_combat_mode)
test('test_character', test_character)
test('test_database', test_database)
# test('test_gamestate_database', test_gamestate_database)
test('test_draw_light', test_draw_light)
test('test_damage', test_damage)
test('test_ringbuffer', test_ringbuffer)
test('test_memory_management', test_memory_management)
#test('test_map_generator', test_map_generator)
#test('test_map_mode', test_map_mode)
test('test_stats', test_stats)
