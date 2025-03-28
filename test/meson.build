# Include test files
test_files = files(
    'test_game.c',
    'test_map_mode.c'
)

# Include files that need to be tested
helper_files = files(
    '../include/termbox2.c',
    '../src/game.c',
    '../src/map.c',
    '../src/map_mode.c',
    '../src/map_generator.c',
    '../src/map_populator.c',
    '../debug/debug.c'
)


test_exe = executable('_test', test_files, helper_files)

test('DungeonCrawl_test', test_exe, args: [])
