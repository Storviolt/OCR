#ifndef ROTATE_AUTOMATICALLY_H
#define ROTATE_AUTOMATICALLY_H

#include <SDL2/SDL.h>

// Detects the required rotation angle of the given SDL surface.
double detect_rotation_angle(SDL_Surface *surface);

// Applies automatic rotation to an SDL_Surface based on detected angle.
void apply_rotate_automatically(SDL_Surface **surface);

#endif

