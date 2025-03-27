#ifndef DEBUG_H
#define DEBUG_H

void db_printEventStruct(int x, int y, const struct tb_event *event);

void print_text(int x, int y, const char *format, ...);

#endif
