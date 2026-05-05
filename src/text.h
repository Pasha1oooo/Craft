#ifndef TEXT_H
#define TEXT_H

#include <notcurses/notcurses.h>

void print2menu(struct ncplane *child_plane, char *str);
void putchar2menu(struct ncplane *child_plane, char ch, struct text_pos *pos);

#endif
