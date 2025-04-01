# Include files that need to be tested
helper_files = files(
    '../src/game.c',
    '../src/combat_mode.c',
    '../src/character_stats.c',
    '../src/items.c',
    '../include/termbox2.c',
    '../src/game.c',
    '../src/map/map.c',
    '../src/map/map_mode.c',
    '../src/map/map_generator.c',
    '../src/map/map_populator.c',
    '../src/map/drawop/draw_light.c',
    '../debug/debug.c',
    '../include/sqlite3.c',
    '../src/database/database.c',
    '../src/logging/logger.c',
    '../src/logging/ringbuffer.c'
)


test_database = executable('test_database', 'test_database.c', helper_files, c_args: ['-w'])
test_draw_light = executable('test_draw_light', 'map/drawop/test_draw_light.c', helper_files, c_args: ['-w'])
test_combat = executable('test_combat_mode', 'test_combat_mode.c', helper_files, c_args: ['-w'])

test('test_database', test_database)
test('test_draw_light', test_draw_light)
test('test_combat_mode', test_combat)
