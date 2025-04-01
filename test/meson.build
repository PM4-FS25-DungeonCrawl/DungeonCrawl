# Include test files
test_files = files(
    'test_game.c',
    'test_main.c',
    'map/drawop/test_draw_light.c',
    'test_database.c'
)

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


test_exe = executable('_test', test_files, helper_files, c_args: ['-w'])
test_exe_combat = executable('test_combat_mode', 'test_combat_mode.c', helper_files, c_args: ['-w'])

test('DungeonCrawl_test', test_exe)
test('test_combat_mode', test_exe_combat)
