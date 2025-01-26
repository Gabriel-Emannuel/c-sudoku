#include "util.h"

board generate_sudoku_board(int d) 
{
    board sudoku = (board) malloc(d * sizeof(line_board));

    for (int i = 0; i < d; i++)
    {
        sudoku[i] = (line_board) malloc(d * sizeof(int));
    }

    for (int i = 0; i < d; i++) 
    {
        for (int j = 0; j < d; j++)
        {
            sudoku[i][j] = 0;
        }
    }

    return sudoku;
}

int get_dimension(void* array)
{
    return sizeof(array) / sizeof(array[0]);
}

int witch_quadrant(int line, int col, int dimension) 
{
    int sqroot = (int) sqrt(dimension);

    int by_line = sqroot * (line / sqroot);

    int by_col = col / sqroot;

    return by_line + by_col;
}