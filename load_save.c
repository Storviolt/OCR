#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <err.h>

// Function to load an image
SDL_Surface* load_image(const char *filename) {
    SDL_Surface *loadedSurface = IMG_Load(filename);
    if (!loadedSurface) {
        errx(EXIT_FAILURE, "Error loading image: %s", IMG_GetError());
    }

    // Convert the image to 32-bit RGBA format to simplify pixel access
    SDL_Surface *optimizedSurface = 
    SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(loadedSurface);
    if (!optimizedSurface) {
        errx(EXIT_FAILURE, "Error converting image: %s", SDL_GetError());
    }

    return optimizedSurface;
}

// Function to save an image
void save_image(SDL_Surface *surface, const char *filename) {
    if (IMG_SavePNG(surface, filename) != 0) {
        errx(EXIT_FAILURE, "Error saving image: %s", IMG_GetError());
    }
}
