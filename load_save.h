#ifndef LOAD_SAVE_H
#define LOAD_SAVE_H

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <err.h>

// Fonction pour charger une image
SDL_Surface* load_image(const char *filename);

// Fonction pour sauvegarder une image
void save_image(SDL_Surface *surface, const char *filename);

#endif // LOAD_SAVE_H
