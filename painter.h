#ifndef PAINTER_H
#define PAINTER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

// Declaration of the function that returns a specific red color
Uint32 rcolor(SDL_PixelFormat *format, int i);

// Declaration of the function that draws a line 
// between two points (x, y) and (x2, y2)
void draw_line(SDL_Surface *surface, int x, int y, int x2, int y2, int i);

// Declaration of the function that draws a shape 
// between two points (x, y) and (x2, y2) with a given size
void draw(SDL_Surface *surface, int x, int y, int x2, int y2, int size);

// Declaration of the function that detects an area of a surface
// between (x, y) and (x2, y2)
void detect(SDL_Surface *surface, int x, int y, int x2, int y2, int size);

#endif 
