
#include "black_and_white.h"

void apply_black_and_white(SDL_Surface *surface) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            Uint32 pixel = pixels[y * surface->w + x];
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

            Uint8 luminance = 0.3 * r + 0.59 * g + 0.11 * b;
            Uint8 bw_color = luminance > 127 ? 255 : 0;
            pixels[y * surface->w + x] = SDL_MapRGBA(surface->format, bw_color, bw_color, bw_color, a);
        }
    }
}
