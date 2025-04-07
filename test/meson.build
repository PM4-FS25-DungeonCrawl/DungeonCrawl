# needed files for each test
helper_db = files(
    '../include/sqlite3.c',
    '../src/database/database.c',
    '../src/database/localization/localization_database.c'
)

helper_draw_light = files(
    '../src/map/map.c',
    '../src/map/drawop/draw_light.c'
)

helper_combat = files(
    '../include/termbox2.c',
    '../src/combat_mode.c',
    '../src/character_stats.c',
    '../src/items.c'
)

# needed directories for each test
incdir_database = include_directories(
    '../test/'
)

# executables
test_database = executable('test_database', 'test_database.c', helper_db, c_args: ['-w'], include_directories: incdir_database)
test_draw_light = executable('test_draw_light', 'map/drawop/test_draw_light.c', helper_draw_light, c_args: ['-w'])
test_combat = executable('test_combat_mode', 'test_combat_mode.c', helper_combat, c_args: ['-w'])

# test
test('test_database', test_database)
test('test_draw_light', test_draw_light)
test('test_combat_mode', test_combat)
