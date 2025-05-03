#include <notcurses/notcurses.h>

// Mock variables for notcurses to satisfy linker for tests
struct notcurses* nc = NULL;
struct ncplane* stdplane = NULL;