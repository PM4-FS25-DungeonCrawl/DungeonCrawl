#include "../include/termbox2.h"
#include "../src/game.h"

void db_printEventStruct(int x, int y,struct tb_event *event) {
    tb_printf(x, y, COLOR_FOREGROUND, COLOR_BACKGROUND, "Type: %d", event->type);
    tb_printf(x, y+1, COLOR_FOREGROUND, COLOR_BACKGROUND, "Modifier: %d", event->mod);
    tb_printf(x, y+2, COLOR_FOREGROUND, COLOR_BACKGROUND, "Key: %d", event->key);
    tb_printf(x, y+3, COLOR_FOREGROUND, COLOR_BACKGROUND, "Character: %c", (char) event->ch);
    tb_printf(x, y+4, COLOR_FOREGROUND, COLOR_BACKGROUND, "Width: %d", event->w);
    tb_printf(x, y+5, COLOR_FOREGROUND, COLOR_BACKGROUND, "Height: %d", event->h);
    tb_printf(x, y+6, COLOR_FOREGROUND, COLOR_BACKGROUND, "Mouse x: %d", event->x);
    tb_printf(x, y+7, COLOR_FOREGROUND, COLOR_BACKGROUND, "Mouse y: %d", event->y);
}
