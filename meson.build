# Define project language and version.
project('DungeonCrawl', 'c',
        version : '0.1',
        default_options : ['c_std=gnu2x','warning_level=3','debug=true'])
add_project_arguments('-Wno-incompatible-pointer-types', language : 'c')
# List of external dependencies
notcurses = dependency('notcurses', required: true)

# List of all source files. Per meson convention all files are listed seperately.
src_files = files(
    'src/main.c',
    'src/common.c',
    'src/game.c',
    'src/game_data.c',

    'src/map/map.c',
    'src/map/map_mode.c',
    'src/map/map_generator.c',
    'src/map/map_populator.c',

    'src/map/draw/draw_light.c',
    'src/map/draw/draw_map_mode.c',

    'src/character/character.c',
    'src/character/player.c',
    'src/character/monster.c',
    'src/character/level.c',

    'src/item/gear.c',
    'src/item/potion.c',
)

menu_files = files(
    'src/menu/main_menu.c',
    'src/menu/save_menu.c',
    'src/menu/menu.c',
    'src/menu/language_menu.c',
    'src/menu/local/save_menu_local.c',
    'src/menu/local/main_menu_local.c',
    'src/menu/local/language_menu_local.c',
)

stats_files = files(
    'src/stats/stats.c',
    'src/stats/stats_mode.c',
    'src/stats/draw/draw_stats.c',
    'src/stats/local/stats_mode_local.c',
)

inventory_files = files(
    'src/inventory/inventory_mode.c',
    'src/inventory/draw/draw_inventory_mode.c',
    'src/inventory/local/inventory_mode_local.c'
)

combat_files = files(
    'src/combat/combat_mode.c',
    'src/combat/ability.c',
    'src/combat/damage.c',
    'src/combat/draw/draw_combat_mode.c',
    'src/combat/local/combat_mode_local.c'
)

# List of all database files
database_files = files(
    'include/sqlite3.c',
    'src/database/database.c',
    'src/database/game/ability_database.c',
    'src/database/game/gamestate_database.c',
    'src/database/game/item_database.c'
)

logging_files = files(
    'src/thread/thread_handler.c',
    'src/logging/logger.c',
    'src/logging/ringbuffer.c'
)


local_files = files(
    'src/local/local_handler.c'
)

memory_files = files(
  'src/memory/memory_management.c'
)

# Include the test directory in the build.
subdir('test')

# Define the executable
executable(
    'DungeonCrawl',
    src_files,
    menu_files,
    stats_files,
    inventory_files,
    combat_files,
    database_files,
    logging_files,
    local_files,
    memory_files,
    dependencies: [notcurses] 
)
