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
    '../src/combat/character.c',
    '../src/combat/damage.c',
    '../src/combat/npc.c',
    '../src/combat/pc.c',

    '../src/items/equipable_item.c',
    '../src/items/usable_item.c',

    '../include/termbox2.c',
    '../src/logging/logger.c',
    '../src/logging/ringbuffer.c'
)

test_database = executable('test_database', 'test_database.c', helper_db, c_args: ['-w'])
test_draw_light = executable('test_draw_light', 'map/drawop/test_draw_light.c', helper_draw_light, c_args: ['-w'])
test_combat = executable('test_combat_mode', 'combat/test_combat_mode.c', helper_combat, c_args: ['-w'])

test('test_database', test_database)
test('test_draw_light', test_draw_light)
test('test_combat_mode', test_combat)
