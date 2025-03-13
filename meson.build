# Define project language and version.
project('rogue', 'c',
  version : '0.1',
  default_options : ['c_std=gnu2x','warning_level=3','debug=true'])

# List of external dependencies
# termbox = dependency('termbox2')

# List of all source files. Per meson convention all files are listed seperately.
src_files = files(
  'src/main.c',
  'src/game.c',
  'include/termbox2.c'
)

debug_files = files(
  'debug/debug.c'
)

# Include the test directory in the build.
subdir('test')

# Define the executable
executable(
  'rogue',
  src_files, 
  debug_files,
)
