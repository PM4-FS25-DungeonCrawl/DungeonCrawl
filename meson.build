# Define project language and version.
project('DungeonCrawl', 'c',
  version : '0.1',
  default_options : ['c_std=gnu2x','warning_level=3','debug=true'])

# List of external dependencies
# termbox = dependency('termbox2')


# List of all source files. Per meson convention all files are listed seperately.
src_files = files(
  'src/main.c',
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

logging_files = files(
  'src/logging/logger.c',
  'src/logging/ringbuffer.c'
)

# Include the test directory in the build.
subdir('test')

# Define the executable
executable(
  'DungeonCrawl',
  src_files,
  include_files,
  debug_files,
  logging_files
)
