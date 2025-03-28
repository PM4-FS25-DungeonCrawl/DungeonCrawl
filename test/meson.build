# Include test files
test_files = files(
    'test_game.c',
    'test_map_mode.c'
)

# Include files that need to be tested
helper_files = files(
    '../src/game.c',
    '../src/map_mode.c',
    '../src/map_generator.c',
    '../src/map_populator.c',
    '../include/termbox2.c',
    '../debug/debug.c'
)


test_exe = executable('test_rogue', test_files, helper_files)

test('DungeonCrawl_test', test_exe)
