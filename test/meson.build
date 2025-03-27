# Include test files
test_files = files(
    'test_game.c',
    'test_database.c'
)

# Include files that need to be tested
helper_files = files(
    '../src/game.c',
    '../src/map_mode.c',
    '../include/termbox2.c',
    '../debug/debug.c',
    '../database/database.c',
    '../include/sqlite3.c',
)


test_exe = executable('test_rogue', test_files, helper_files)

test('DungeonCrawl_test', test_exe)
