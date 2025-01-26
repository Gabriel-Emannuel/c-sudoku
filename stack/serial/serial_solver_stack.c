#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef int board[9][9];
typedef bool mis_numbers[9][10];

typedef struct {
    int fst;
    int snd;
    int thd;
} trio;

bool is_resolved(board);
bool fill_board(int, bool[][][]);

int get_dimensions_sudoku(board);
int get_quadrant(int, int);

int resolve_sudoku(board);

trio get_possible_place(board, bool[][][]);

void generate_quadrant_numbers(board, int);
void generate_collum_mis_numbers(board, int);
void generate_line_mis_numbers(board, int);
void generate_intersection_numbers(mis_numbers, mis_numbers, mis_numbers, int);
void try_fill_board(board, trio, bool[], int);

int main()
{

    int sudoku[9][9];

    return EXIT_SUCCESS;
}

int resolve_sudoku(board sudoku) 
{
    mis_numbers cols, lines, quadrants;

    bool mis_numbers_board[9][9][10],
         is_filled;

    trio possible_place;

    while (is_resolved(sudoku)) 
    {
        generate_collum_mis_numbers(sudoku, &cols);
        generate_line_mis_numbers(sudoku, &lines);
        generate_quadrant_numbers(sudoku, &quadrants);

        generate_intersection_numbers(cols, lines, quadrants, &mis_numbers_board);

        is_filled = fill_board(&sudoku, mis_numbers_board);

        if (is_filled) continue;

        possible_place = get_possible_place(sudoku, mis_numbers_board);

        try_fill_board(sudoku, possible_place, mis_numbers_board[possible_place.fst][possible_place.snd], &sudoku);
    }

    return sudoku;
}

void try_fill_board(board sudoku, trio possible_place, bool mis_numbers_board[9], int pointer) {
    
    int possible_numbers[possible_place.thd];

    int index = 0;

    int d = get_dimensions_sudoku(sudoku);

    for (int k = 1; k < d + 1; k++) 
    {
        if (index == possible_place.thd) 
        {
            break;
        }
        if (mis_numbers_board[k]) 
        {
            possible_numbers[index] = k;
            index++;
        }
    }

    

}

bool fill_board(int pointer, bool mis_numbers_board[9][9][10]) 
{
    board sudoku = &pointer;

    int dimension = get_dimensions_sudoku(sudoku);

    bool is_filled = false;

    int number_possible;

    for (int i = 0; i < dimension; i++) 
    {
        for (int j = 0; j < dimension; j++) 
        {
            if (sudoku[i][j] != 0) continue;
            number_possible = 0;
            
            for (int k = 0; k < dimension; k++) 
            {
                if (mis_numbers_board[i][j][k+1]) 
                {
                    if (number_possible != 0) 
                    {
                        break;
                    }
                    number_possible = k+1;
                }
            }
            
            sudoku[i][j] = number_possible;
            is_filled = true;
        }
    }

    return is_filled;
}

trio get_possible_place(board sudoku, bool mis_numbers_board[9][9][10]) 
{
    trio possible_location;

    possible_location.thd = 10;

    int d = get_dimensions_sudoku(sudoku);

    int number_board;

    for (int i = 0; i < d; i++) 
    {
        for (int j = 0; j < d; j++) 
        {
            if (sudoku[i][j] != 0)
            {
                continue;
            } 
            
            number_board = 0;
    
            for (int k = 0; k < d; k++) 
            {
                if (mis_numbers_board[i][j][k+1]) 
                {
                    number_board++;
                }    
            }
            
            if (number_board < possible_location.thd) 
            {
                possible_location.fst = i;
                possible_location.snd = j;
                possible_location.thd = number_board;
            }
        }
    }

    return possible_location;

}

void generate_intersection_numbers(mis_numbers cols, mis_numbers lines, mis_numbers quadrants, int pointer) 
{
    bool mis_numbers_board[9][9][9] = &pointer; 

    int quadrant, size_sudoku;

    int size_sudoku = sizeof(cols);

    for (int i = 0; i < size_sudoku; i++) 
    {
        for (int j = 0; j < size_sudoku; j++) 
        {
            quadrant = get_quadrant(i, j);
            for (int k = 0; k < size_sudoku + 1; k++) 
            {
                mis_numbers_board[i][j][k] = cols[j][k] && lines[i][k] && quadrants[quadrant][k];
            }
        }
    }
}

void generate_collum_mis_numbers(board sudoku, int pointer) 
{
    mis_numbers cols = &pointer;

    int d = get_dimensions_sudoku(sudoku);

    int number;

    for (int j = 0; j < d; j++) 
    {
        for (int i = 0; i < d; i++) 
        {
            number = sudoku[i][j];
            cols[j][number] = true;
        }
    }
}

void generate_line_mis_numbers(board sudoku, int pointer) 
{
    mis_numbers lines = &pointer;

    int d = get_dimensions_sudoku(sudoku);

    int number;

    for (int i = 0; i < d; i++) 
    {
        for (int j = 0; j < d; j++) 
        {
            number = sudoku[i][j];
            lines[i][number] = true;
        }
    }
}

void generate_quadrants_mis_numbers(board sudoku, int pointer) 
{
    mis_numbers quadrants = &pointer;

    int d = get_dimensions_sudoku(sudoku);

    int sqroot = (int) sqrt(d);

    int number, line, col;

    for (int k = 0; k < d; k++) 
    {
        for (int i = 0; i < sqroot; i++) 
        {
            for (int j = 0; j < sqroot; j++) 
            {
                line = i + (k % sqroot);
                col = j + (k / sqroot); 

                number = sudoku[line][col];
            
                quadrants[k][number] = true;
            }
        }
    }

}

int get_dimensions_sudoku(board sudoku) 
{
    return sizeof(sudoku);
}

bool is_resolved(board sudoku) 
{
    bool is_resolved;

    int d = get_dimensions_sudoku(sudoku);

    for (int i = 0; i < d; i++) 
    {
        for (int j = 0; j < d; j++)
        {
            is_resolved = sudoku[i][j] != 0;
            if (!is_resolved) return false;
        }
    }

    return true;
}
