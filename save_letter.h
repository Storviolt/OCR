#ifndef SAVE_LETTER_H
#define SAVE_LETTER_H

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <err.h>

// struct for the coordinate of a letter
typedef struct {
    int x;
    int y;
} Coordinate;

// Function that return the struct coordinate of the letter at the index (i,j)
Coordinate get_coordinate(Coordinate *coordinates, int rows, int cols, int i,\
 int j);

// Function that save the number of row and columns 
// ( for the array of coordinate)
// depending the coordinate y of letters
void update_rows_and_cols(Coordinate *coordinates, int letter_count,\
 int *rows, int *cols);

// Functiun that extract and save the letters of an image,
// and return Coordinate* the array of the coordinate of the letters
Coordinate* extract_and_save_letters(SDL_Surface *surface,\
 const char *output_dir, int *letter_count, int *rows, int *cols);

#endif // SAVE_LETTER_H
