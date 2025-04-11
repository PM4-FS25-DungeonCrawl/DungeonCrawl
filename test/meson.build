# needed files for each test
helper_db = files(
    '../include/sqlite3.c',
    '../src/database/database.c')

helper_draw_light = files(
    '../src/map/map.c',
    '../src/map/drawop/draw_light.c'
)

helper_combat = files(
    '../src/combat/combat_mode.c',
    '../src/combat/ability.c',
    '../src/combat/damage.c',
    '../src/combat/display/combat_display.c',

    '../src/character/character.c',
    '../src/character/level.c',
    '../src/character/player.c',
    '../src/character/monster.c',

    '../src/item/gear.c',
    '../src/item/potion.c',

    '../include/termbox2.c',
    '../src/logging/logger.c',
    '../src/logging/ringbuffer.c'
)

helper_ringbuffer = files(
    '../src/logging/ringbuffer.c'
)

test_database = executable('test_database', 'test_database.c', helper_db, c_args: ['-w'])
test_draw_light = executable('test_draw_light', 'map/drawop/test_draw_light.c', helper_draw_light, c_args: ['-w'])
test_damage = executable('test_damage', 'combat/test_damage.c', helper_combat, c_args: ['-w'])
test_ringbuffer = executable('test_ringbuffer', 'logging/test_ringbuffer.c', helper_ringbuffer, c_args: ['-w'])

test('test_database', test_database)
test('test_draw_light', test_draw_light)
test('test_damage', test_damage)
test('test_ringbuffer', test_ringbuffer)
