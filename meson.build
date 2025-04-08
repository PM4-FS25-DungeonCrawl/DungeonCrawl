# Define project language and version.
project('DungeonCrawl', 'c',
        version : '0.1',
        default_options : ['c_std=gnu2x','warning_level=3','debug=true'])

# List of external dependencies
# termbox = dependency('termbox2')


# List of all source files. Per meson convention all files are listed seperately.
src_files = files(
    'src/main.c',
    'src/combat_mode.c',
    'src/character_stats.c',
    'src/game.c',
    'src/items.c',
    'src/game.c',
    'src/map/map.c',
    'src/map/map_mode.c',
    'src/map/map_generator.c',
    'src/map/map_populator.c',
    'src/map/drawop/draw_light.c'
)

include_files = files(
    'include/termbox2.c'
)

debug_files = files(
    'debug/debug.c'
)

# List of all database files
database_files = files(
    'include/sqlite3.c',
    'src/database/database.c',
    'src/database/local/local_database.c'
)

logging_files = files(
    'src/logging/logger.c',
    'src/logging/ringbuffer.c'
)

local_files = files(
    'src/local/local.c'
)

# Include the test directory in the build.
subdir('test')

# Define the executable
executable(
    'DungeonCrawl',
    src_files,
    include_files,
    debug_files,
    database_files,
    logging_files,
    local_files,
)
