# Include test files
test_files = files(
    'test_main.c',
    'map/drawop/test_draw_light.c',
    'test_database.c'
)

# Include files that need to be tested
helper_files = files(
    '../include/termbox2.c',
    '../src/game.c',
    '../src/map/map.c',
    '../src/map/map_mode.c',
    '../src/map/map_generator.c',
    '../src/map/map_populator.c',
    '../src/map/drawop/draw_light.c',
    '../debug/debug.c',
    '../include/sqlite3.c',
    '../src/database/database.c'
)


test_exe = executable('_test', test_files, helper_files)

test('DungeonCrawl_test', test_exe, args: [])
