#include "preprocess.h"
#include <SDL2/SDL.h>

#include <SDL2/SDL_image.h>

#include <stdio.h>

#include <stdlib.h>

#include <math.h>


#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))


// Fonction pour augmenter le contraste

void increaseContrast(SDL_Surface *surface) {

    Uint32 *pixels = (Uint32 *)surface->pixels;

    int width = surface->w;

    int height = surface->h;


    // Trouver les valeurs min et max

    int min = 255, max = 0;

    for (int y = 0; y < height; y++) {

        for (int x = 0; x < width; x++) {

            Uint32 pixel = pixels[y * width + x];

            Uint8 r, g, b;

            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            if (r < min) min = r;

            if (g < min) min = g;

            if (b < min) min = b;

            if (r > max) max = r;

            if (g > max) max = g;

            if (b > max) max = b;

        }

    }


    // Appliquer le contraste

    for (int y = 0; y < height; y++) {

        for (int x = 0; x < width; x++) {

            Uint32 pixel = pixels[y * width + x];

            Uint8 r, g, b;

            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            r = CLAMP((r - min) * 255 / (max - min), 0, 255);

            g = CLAMP((g - min) * 255 / (max - min), 0, 255);

            b = CLAMP((b - min) * 255 / (max - min), 0, 255);

            pixels[y * width + x] = SDL_MapRGB(surface->format, r, g, b);

        }

    }

}


// Fonction pour appliquer le filtre médian

void applyMedianFilter(SDL_Surface *surface) {

    Uint32 *pixels = (Uint32 *)surface->pixels;

    int width = surface->w;

    int height = surface->h;

    Uint32 *temp = (Uint32 *)malloc(width * height * sizeof(Uint32));


    for (int y = 1; y < height - 1; y++) {

        for (int x = 1; x < width - 1; x++) {

            Uint8 r[9], g[9], b[9];

            int index = 0;


            // Collecter les valeurs des pixels voisins

            for (int dy = -1; dy <= 1; dy++) {

                for (int dx = -1; dx <= 1; dx++) {

                    Uint32 pixel = pixels[(y + dy) * width + (x + dx)];

                    SDL_GetRGB(pixel, surface->format, &r[index], &g[index], &b[index]);

                    index++;

                }

            }


            // Trier les valeurs pour trouver la médiane

            for (int i = 0; i < 9; i++) {

                for (int j = i + 1; j < 9; j++) {

                    if (r[i] > r[j]) {

                        Uint8 tempR = r[i]; r[i] = r[j]; r[j] = tempR;

                        Uint8 tempG = g[i]; g[i] = g[j]; g[j] = tempG;

                        Uint8 tempB = b[i]; b[i] = b[j]; b[j] = tempB;

                    }

                }

            }


            // Prendre la médiane

            temp[y * width + x] = SDL_MapRGB(surface->format, r[4], g[4], b[4]);

        }

    }


    // Copier les pixels traités dans l'image d'origine

    for (int y = 1; y < height - 1; y++) {

        for (int x = 1; x < width - 1; x++) {

            pixels[y * width + x] = temp[y * width + x];

        }

    }


    free(temp);

}
void apply_preprocess(SDL_Surface *surface) {
	increaseContrast(surface);
    applyMedianFilter(surface);
} 
