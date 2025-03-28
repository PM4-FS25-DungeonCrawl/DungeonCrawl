# Include test files
test_files = files(
    'test_game.c',
)

# Include files that need to be tested
helper_files = files(
    '../src/game.c',
    '../src/map_mode.c',
    '../src/combat_mode.c',
    '../src/character_stats.c',
    '../include/termbox2.c',
    '../debug/debug.c'
)


test_exe = executable('test_rogue', test_files, helper_files, c_args: ['-w'])
test_exe_combat = executable('test_combat_mode', 'test_combat_mode.c', helper_files, c_args: ['-w'])

test('DungeonCrawl_test', test_exe)
test('test_combat_mode', test_exe_combat)