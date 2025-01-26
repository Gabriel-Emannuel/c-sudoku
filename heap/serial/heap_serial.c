#include <stdio.h>
#include <math.h>
#include "heap_serial.h"

board copy_sudoku(board);
board generate_possible_values(mis_numbers**);
board try_fill_sudoku(board, trio, mis_numbers**);
board resolve_sudoku_heap_serial(board);

bool fill_sudoku(board, board);
bool is_resolved(board);
bool is_valid_possible_values(board);
bool is_valid_cols(board);
bool is_valid_lines(board);
bool is_valid_quadrants(board);

mis_numbers* generate_cols_mis_numbers(board);
mis_numbers* generate_line_mis_numbers(board);
mis_numbers* generate_quadrant_mis_numbers(board);

mis_numbers* generate_mis_numbers_array(int);
mis_numbers** generate_mis_numbers_matrix(int);

mis_numbers** generate_intersections(mis_numbers*, mis_numbers*, mis_numbers*);

trio generate_possible_place(board, mis_numbers**);

void free_all(mis_numbers*, mis_numbers*, mis_numbers*);
void free_board(board);
void free_intersections(mis_numbers**);
void free_mis_numbers(mis_numbers*);

void generate_invalid(board);

bool is_valid_possible_values(board sudoku)
{
    return is_valid_lines(sudoku) && is_valid_cols(sudoku) && is_valid_quadrants(sudoku);
}

bool is_valid_lines(board sudoku) 
{
    int d = get_dimension(sudoku);

    bool numbers_per_line[d][d];

    int number;

    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            numbers_per_line[i][j] = false;
        }
    }

    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            number = sudoku[i][j];
            if (numbers_per_line[i][number-1])
            {
                return false;
            }
            numbers_per_line[i][number-1] = true;
        }
    }

    return true;
}

bool is_valid_cols(board sudoku)
{
    int d = get_dimension(sudoku);

    bool numbers_per_cols[d][d];

    int number;

    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            numbers_per_cols[i][j] = false;
        }
    }

    for (int j = 0; j < d; j++)
    {
        for (int i = 0; i < d; i++)
        {
            number = sudoku[i][j];
            if (numbers_per_cols[j][number-1])
            {
                return false;
            }
            numbers_per_cols[i][number-1] = true;
        }
    }

    return true;
}

bool is_valid_quadrants(board sudoku)
{
    int d = get_dimension(sudoku);

    bool numbers_per_quadrant[d][d];

    int number, quadrant;

    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            numbers_per_quadrant[i][j] = false;
        }
    }

    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            quadrant = witch_quadrant(i, j, d);

            number = sudoku[i][j];

            if (numbers_per_quadrant[quadrant][number-1])
            {
                return false;
            }
            numbers_per_quadrant[quadrant][number-1] = true;
        }
    }

    return true;
}

bool is_resolved(board sudoku) 
{
    int d = get_dimension(sudoku);

    if (sudoku[0][0] == d)
    {
        return false;
    }

    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            if (!sudoku[i][j])
            {
                return false;
            }
        }
    }

    return true;
}

board resolve_sudoku_heap_serial(board sudoku) 
{
    mis_numbers* mis_per_line, 
                 mis_per_col, 
                 mis_per_quadrant;

    mis_numbers** intersections;

    bool is_filled;

    trio possible_place;

    board possible_values;

    while (!is_resolved(sudoku)) 
    {
        mis_per_col = generate_cols_mis_numbers(sudoku);
        mis_per_line = generate_line_mis_numbers(sudoku);
        mis_per_quadrant = generate_quadrant_mis_numbers(sudoku);

        intersections = generate_intersections(mis_per_col, mis_per_line, mis_per_quadrant);
        
        free_all(mis_per_col, mis_per_line, mis_per_quadrant);

        possible_values = generate_possible_values(intersections);

        if (!is_valid_possible_values(possible_values)) 
        {
            free_board(possible_values);
            free_intersections(intersections);

            generate_invalid(sudoku);
            return sudoku;
        }

        is_filled = fill_sudoku(sudoku, possible_values);

        if (is_filled) 
        {
            free_intersections(intersections);
            free_board(possible_values);
            continue;
        }

        possible_place = generate_possible_place(sudoku, intersections);

        return try_fill_sudoku(sudoku, possible_place, intersections);
    }

    return sudoku;

}

board copy_sudoku(board sudoku) 
{
    int d = get_dimension(sudoku);

    board copy = generate_sudoku_board(d);

    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            copy[i][j] = sudoku[i][j];
        }
    }

    return copy;
}

board try_fill_sudoku(board sudoku, trio possible_place, mis_numbers** intersections) 
{
    board sudoku_copy;

    int dimension = get_dimension(sudoku);

    int possible_numbers[possible_place.thd];

    int index = 0;

    for (int i = 0; i < sudoku; i++) 
    {
        if (index == possible_place.thd) 
        {
            break;
        }

        if (intersections[possible_place.fst][possible_place.snd][i]) 
        {
            possible_numbers[index] = i + 1;
            index++;
        }
    }

    free_intersections(intersections);

    for (int i = 0; i < possible_place.thd; i++)
    {
        sudoku_copy = copy_sudoku(sudoku);
        sudoku_copy[possible_place.fst][possible_place.snd] = possible_numbers[i];
        sudoku_copy = resolve_sudoku_heap_serial(sudoku_copy);

        if (is_resolved(sudoku_copy))
        {
            free_board(sudoku);
            return sudoku_copy;
        }

        free_board(sudoku_copy);
    }

    generate_invalid(sudoku);

    return sudoku;

}

board generate_possible_values(mis_numbers** intersections)
{
    int d = get_dimension(intersections);

    board possible_values = generate_sudoku_board(d);

    int number;

    for (int i = 0; i < d; i++) 
    {
        for (int j = 0; j < d; j++)
        {
            number = 0;
            for (int k = 0; k < d; k++) 
            {
                if (intersections[i][j][k])
                {
                    if (number != 0)
                    {
                        number = 0;
                        break;
                    }
                    number = k + 1;
                }
            }
            possible_values[i][j] = number;
        }
    }

    return possible_values;
    
}

void generate_invalid(board sudoku)
{
    int d = get_dimension(sudoku);

    for (int i = 0; i < d; i++) 
    {
        for (int j = 0; j < d; j++) 
        {
            sudoku[i][j] = d;
        }
    }
}

mis_numbers* generate_mis_numbers_array(int d) 
{
    mis_numbers* array = (mis_numbers*) malloc(d * sizeof(mis_numbers));

    for (int i = 0; i < d; i++) 
    {
        array[i] = malloc(d * sizeof(bool));
    }

    for (int i = 0; i < d; i++) 
    {
        for (int j = 0; j < d; j++) 
        {
            array[i][j] = true;
        }
    }

    return array;

}

mis_numbers** generate_mis_numbers_matrix(int d) 
{
    mis_numbers** matrix = (mis_numbers**) malloc(d * sizeof(mis_numbers*));

    for (int i = 0; i < d; i++) 
    {
        matrix[i] = generate_mis_numbers_array(d);
    }

    return matrix;
}

trio generate_possible_place(board sudoku, mis_numbers** intersections) 
{
    trio possible_place;

    possible_place.thd = 10;

    int number_possible;

    int dimensions = get_dimension(sudoku);

    for (int i = 0; i < dimensions; i++) 
    {
        for (int j = 0; j < dimensions; j++) 
        {
            if (sudoku[i][j]) 
            {
                continue;
            }

            number_possible = 0;

            for (int k = 0; k < dimensions; k++) 
            {
                if (intersections[i][j][k]) {
                    number_possible++;
                }
            }
            
            if (number_possible < possible_place.thd) 
            {
                possible_place.fst = i;
                possible_place.snd = j;
                possible_place.thd = number_possible;
            }
        }
    }

    return possible_place;

}

bool fill_sudoku(board sudoku, board possible_values) 
{
    int dimension = get_dimension(sudoku);

    bool is_filled = false;

    for (int i = 0; i < dimension; i++) 
    {
        for (int j = 0; j < dimension; j++) 
        {
            if (sudoku[i][j] || !possible_values[i][j]) 
            {
                continue;
            }

            sudoku[i][j] = possible_values[i][j];
            
            is_filled = true;
        }
    }

    return is_filled;

}

mis_numbers** generate_intersections(mis_numbers* mis_per_col, mis_numbers* mis_per_line, mis_numbers* mis_per_quadrant) 
{
    int dimension = get_dimension(mis_per_col);

    mis_numbers** intersections = generate_mis_numbers_matrix(dimension);

    mis_numbers col, line, quadrant;

    for (int i = 0; i < dimension; i++) 
    {
        for (int j = 0; j < dimension; j++) 
        {
            col = mis_per_col[j];
            line = mis_per_line[i];
            quadrant = mis_per_quadrant[witch_quadrant(i, j, dimension)];

            for (int k = 0; k < dimension; k++) 
            {
                intersections[i][j][k] = col[k] && line[k] && quadrant[k];
            }
        }
    }

    return intersections;
}

mis_numbers* generate_cols_mis_numbers(board sudoku) 
{
    int dimension = get_dimension(sudoku);

    mis_numbers* mis_per_col = generate_mis_numbers_array(dimension);

    int number;

    for (int j = 0; j < dimension; j++) 
    {
        for (int i = 0; i < dimension; i++) 
        {
            number = sudoku[i][j];
            if (number != 0) 
            {
                mis_per_col[j][number-1] = false;
            }
        }
    }

    return mis_per_col;
}

mis_numbers* generate_line_mis_numbers(board sudoku) 
{
    int dimension = get_dimension(sudoku);

    mis_numbers* mis_per_line = generate_mis_numbers_array(dimension);

    int number;

    for (int i = 0; i < dimension; i++) 
    {
        for (int j = 0; j < dimension; j++) 
        {
            number = sudoku[i][j];
            if (number != 0) 
            {
                mis_per_line[i][number-1] = false;
            }
        }
    }

    return mis_per_line;
}

mis_numbers* generate_quadrant_mis_numbers(board sudoku) 
{
    int dimension = get_dimension(sudoku);

    mis_numbers* mis_per_quadrant = generate_mis_numbers_array(dimension);

    int sqroot = (int) sqrt(dimension);

    int col, line, number;

    for (int k = 0; k < dimension; k++) 
    {
        for (int i = 0; i < sqroot; i++) 
        {
            for (int j = 0; j < sqroot; j++) 
            {
                line = i + (sqroot * (k / sqroot));
                col = j + (sqroot * (k % sqroot));
                number = sudoku[line][col];

                if (number != 0) 
                {
                    mis_per_quadrant[k][number] = false;
                }
            }
        }
    }

    return mis_per_quadrant;
}

void free_board(board sudoku) 
{
    int dimension = get_dimension(sudoku);

    for (int i = 0; i < dimension; i++) 
    {
        free(sudoku[i]);
    }

    free(sudoku);
}

void free_intersections(mis_numbers** intersections) 
{
    int lines = sizeof(intersections) / sizeof(intersections[0]);
    int cols = sizeof(intersections[0]) / sizeof(intersections[0][0]);

    for (int i = 0; i < lines; i++) 
    {
        free_mis_numbers(intersections[i]);
    }

    free(intersections);
}

void free_all(mis_numbers* array1, mis_numbers* array2, mis_numbers* array3)
{
    free_mis_numbers(array1);
    free_mis_numbers(array2);
    free_mis_numbers(array3);
}

void free_mis_numbers(mis_numbers* numbers) 
{
    int size = sizeof(numbers) / sizeof(numbers[0]);

    for (int i = 0; i < size; i++) 
    {
        free(numbers[i]);
    }

    free(numbers);
}