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

    'src/character/character.c',
    'src/character/player.c',
    'src/character/monster.c',
    'src/character/level.c',

    'src/inventory/inventory_mode.c',

    'src/item/gear.c',
    'src/item/potion.c',

    'src/menu/main_menu.c',
    'src/menu/save_menu.c',
    'src/menu/menu.c',
    'src/menu/language_menu.c',

    'src/combat/combat_mode.c',
    'src/combat/ability.c',
    'src/combat/damage.c',

    'src/stats/stats.c',
    'src/stats/stats_mode.c'
)

# List of all database files
database_files = files(
    'include/sqlite3.c',
    'src/database/database.c',
    'src/database/local/local_database.c',
    'src/database/game/gamestate_database.c',
    'src/database/game/ability_database.c',
    'src/database/game/item_database.c'
)

logging_files = files(
    'src/thread/thread_handler.c',
    'src/logging/logger.c',
    'src/logging/ringbuffer.c'
)

io_files = files(
    'src/io/io_handler.c',
    'src/io/input/input_handler.c',
    'src/io/output/common/common_output.c',
    'src/io/output/media/media_output.c',
    'src/io/output/specific/combat_output.c',
    'src/io/output/specific/inventory_output.c',
    'src/io/output/specific/map_output.c',
    'src/io/output/specific/stats_output.c',
    'src/io/output/specific/wait_output.c'
)

local_files = files(
    'src/local/local.c'
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
    database_files,
    logging_files,
    io_files,
    local_files,
    memory_files,
    dependencies: [notcurses]
)
