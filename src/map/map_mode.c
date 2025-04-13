#include <math.h>
#include <notcurses/notcurses.h>
#include <stdio.h>
#include <wchar.h>

#include "../../debug/debug.h"

#include "../input.h"
#include "drawop/draw_light.h"
#include "map.h"
#include "map_mode.h"
#include "src/game.h"
#include "src/logging/logger.h"

map_tile revealed_map[WIDTH][HEIGHT];

Vector2D player_pos;

struct ncplane *stdplane;
struct notcurses *nc;

void set_start(const int newPlayerX, const int newPlayerY) {
  player_pos.dx = newPlayerX;
  player_pos.dy = newPlayerY;

  // at the start, tile under the player must be revealed
  revealed_map[player_pos.dx][player_pos.dy] = FLOOR;
}

void draw_map() {

  ncplane_set_fg_rgb(stdplane, 0xffffff); // white
  ncplane_set_bg_rgb(stdplane, 0x000000); // black
  ncplane_printf_yx(stdplane, 10, 10, "Test");
  log_msg(INFO, "game", "rendered test");
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      if (x == player_pos.dx && y == player_pos.dy) {
        ncplane_set_fg_rgb(stdplane, 0xffffff); // white
        ncplane_set_bg_rgb(stdplane, 0x000000); // black
        ncplane_putchar_yx(stdplane, y, x, '@');
      } else {
        switch (revealed_map[x][y]) {
        case WALL:
          ncplane_set_fg_rgb(stdplane, 0xffffff);
          ncplane_set_bg_rgb(stdplane, 0x000000);
          ncplane_putchar_yx(stdplane, y, x, '#');
          break;
        case FLOOR:
          ncplane_set_fg_rgb(stdplane, 0xffffff);
          ncplane_set_bg_rgb(stdplane, 0x000000);
          ncplane_putchar_yx(stdplane, y, x, ' ');
          break;
        case START_DOOR:
          ncplane_set_fg_rgb(stdplane, 0x00ff00); // green
          ncplane_set_bg_rgb(stdplane, 0x000000);
          ncplane_putchar_yx(stdplane, y, x, '#');
          break;
        case EXIT_DOOR:
          ncplane_set_fg_rgb(stdplane, 0xffff00); // yellow
          ncplane_set_bg_rgb(stdplane, 0x000000);
          ncplane_putchar_yx(stdplane, y, x, '#');
          break;
        case KEY:
          ncplane_set_fg_rgb(stdplane, 0xffff00);
          ncplane_set_bg_rgb(stdplane, 0x000000);
          ncplane_putchar_yx(stdplane, y, x, '$');
          break;
        case SKELETON:
          ncplane_set_fg_rgb(stdplane, 0xffffff);
          ncplane_set_bg_rgb(stdplane, 0xff0000); // red
          ncplane_putchar_yx(stdplane, y, x, '!');
          break;
        case HIDDEN:
          ncplane_set_fg_rgb(stdplane, 0xffffff);
          ncplane_set_bg_rgb(stdplane, 0xffffff);
          ncplane_putchar_yx(stdplane, y, x, ' ');
          break;
        default:
          // TODO: log error
          return;
        }
      }
    }
  }
}

void draw_ui(void) {
  ncplane_set_fg_rgb(stdplane, 0xffffff);
  ncplane_set_bg_rgb(stdplane, 0x000000);
  ncplane_printf_yx(stdplane, HEIGHT, 0, "HP: 100");
  ncplane_printf_yx(stdplane, HEIGHT + 2, 0, "Player Position: %d, %d",
                    player_pos.dx, player_pos.dy);
}

void move_player(enum directions direction) {
  int new_x = player_pos.dx;
  int new_y = player_pos.dy;
  switch (direction) {
  case DIR_UP:
    new_y--;
    break;
  case DIR_DOWN:
    new_y++;
    break;
  case DIR_LEFT:
    new_x--;
    break;
  case DIR_RIGHT:
    new_x++;
    break;
  }
  if (new_x >= 0 && new_x < WIDTH && new_y >= 0 && new_y < HEIGHT) {
    switch (map[new_x][new_y]) {
    case WALL:
      // ignore wall
      break;
    default:
      // TODO: extend functionality with different tiles
      player_pos.dx = new_x;
      player_pos.dy = new_y;

      draw_light_on_player((int *)map, (int *)revealed_map, HEIGHT, WIDTH,
                           player_pos, LIGHT_RADIUS);
      break;
    }
  }
}

/**
 * Updates the player position based on the player's input and redraws the maze.
 * @return CONTINUE (0) if the game continue, QUIT (1) if the player pressed the
 * exit key.
 */
int map_mode_update(void) {

  ncinput *ni = NULL;
  notcurses_get(nc, NULL, ni);

  handle_input(nc, ni, MAP_MODE);

  draw_map();
  draw_ui();

  return CONTINUE;
}

int init_map_mode(struct notcurses *notcurses, struct ncplane *ncplane) {
  nc = notcurses;
  stdplane = ncplane;
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      revealed_map[x][y] = HIDDEN;
    }
  }
  return 0;
}
