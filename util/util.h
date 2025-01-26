#ifndef UTIL_H
#define UTIL_H

#include "../types/types.h"

board generate_sudoku_board(int d);

int get_dimension(void*);
int witch_quadrant(int, int, int);

#endif