# needed files for each test
helper_db = files(
    '../include/sqlite3.c',
    '../src/database/database.c')

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

helper_ringbuffer = files(
    '../src/logging/ringbuffer.c'
)

test_database = executable('test_database', 'test_database.c', helper_db, c_args: ['-w'])
test_draw_light = executable('test_draw_light', 'map/drawop/test_draw_light.c', helper_draw_light, c_args: ['-w'])
test_combat = executable('test_combat_mode', 'test_combat_mode.c', helper_combat, c_args: ['-w'])
test_ringbuffer = executable('test_ringbuffer', 'logging/test_ringbuffer.c', helper_ringbuffer, c_args: ['-w'])

test('test_database', test_database)
test('test_draw_light', test_draw_light)
test('test_combat_mode', test_combat)
test('test_ringbuffer', test_ringbuffer)
