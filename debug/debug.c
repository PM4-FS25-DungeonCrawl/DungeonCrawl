#include <stdarg.h>
#include "../include/termbox2.h"
#include "../src/game.h"

void db_printEventStruct(const int x, const int y, const struct tb_event *event) {
    tb_printf(x, y, COLOR_FOREGROUND, COLOR_BACKGROUND, "Type: %d", event->type);
    tb_printf(x, y+1, COLOR_FOREGROUND, COLOR_BACKGROUND, "Modifier: %d", event->mod);
    tb_printf(x, y+2, COLOR_FOREGROUND, COLOR_BACKGROUND, "Key: %d", event->key);
    tb_printf(x, y+3, COLOR_FOREGROUND, COLOR_BACKGROUND, "Character: %c", (char) event->ch);
    tb_printf(x, y+4, COLOR_FOREGROUND, COLOR_BACKGROUND, "Width: %d", event->w);
    tb_printf(x, y+5, COLOR_FOREGROUND, COLOR_BACKGROUND, "Height: %d", event->h);
    tb_printf(x, y+6, COLOR_FOREGROUND, COLOR_BACKGROUND, "Mouse x: %d", event->x);
    tb_printf(x, y+7, COLOR_FOREGROUND, COLOR_BACKGROUND, "Mouse y: %d", event->y);
}

void print_text(const int x, const int y, const char *format, ...) {
    va_list args;
    va_start(args, format);

    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    // Clear the previous text by printing spaces
    tb_printf(x, y, COLOR_FOREGROUND, COLOR_BACKGROUND, "%*s", (int)sizeof(buffer), " ");


    tb_printf(x, y, COLOR_FOREGROUND, COLOR_BACKGROUND, "%s", buffer);
}
