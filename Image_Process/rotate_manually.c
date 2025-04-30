#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>
#include "rotate_manually.h"

void apply_rotation(SDL_Surface **surface, double angle) {
	// Convert angle to radians
	double radians = angle * M_PI / 180.0;

	// Get the original width and height
	int width = (*surface)->w;
	int height = (*surface)->h;

	// Create a new surface with the same dimensions
	SDL_Surface *rotated_surface = SDL_CreateRGBSurface(
			0, width, height, 32,
			(*surface)->format->Rmask, (*surface)->format->Gmask,
			(*surface)->format->Bmask, (*surface)->format->Amask);

	if (!rotated_surface) {
		fprintf(stderr, "Failed to create rotated surface: %s\n", SDL_GetError());
		return;
	}

	// Calculate the center of the original and new surfaces
	int cx = width / 2;
	int cy = height / 2;

	// Lock the surfaces for pixel manipulation
	SDL_LockSurface(*surface);
	SDL_LockSurface(rotated_surface);

	// Get pixel data
	Uint32 *pixels = (Uint32 *)(*surface)->pixels;
	Uint32 *rotated_pixels = (Uint32 *)rotated_surface->pixels;

	// Clear the new surface with a transparent background
	SDL_memset(rotated_pixels, 0, rotated_surface->pitch * rotated_surface->h);

	// Rotate the image using reverse mapping
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			// Translate the point to the center of rotation
			int dx = x - cx;
			int dy = y - cy;

			// Apply inverse rotation to find the source pixel
			double src_x = dx * cos(-radians) - dy * sin(-radians) + cx;
			double src_y = dx * sin(-radians) + dy * cos(-radians) + cy;

			// Check if the source position is within bounds
			if (src_x >= 0 && src_x < width && src_y >= 0 && src_y < height) {
				// Perform bilinear interpolation
				int x0 = (int)src_x;
				int y0 = (int)src_y;
				int x1 = x0 + 1;
				int y1 = y0 + 1;

				// Clamp coordinates to image bounds
				x1 = (x1 >= width) ? width - 1 : x1;
				y1 = (y1 >= height) ? height - 1 : y1;
				// Get the four surrounding pixels (Uint32 values)
				Uint32 p00 = pixels[y0 * width + x0];
				Uint32 p01 = pixels[y1 * width + x0];
				Uint32 p10 = pixels[y0 * width + x1];
				Uint32 p11 = pixels[y1 * width + x1];


				double a = src_x - x0; // Fractional part in x direction
				double b = src_y - y0; // Fractional part in y direction

				// Rename r, g, b, a variables to avoid conflicts
				Uint8 r, g, blue, a_channel;
				Uint8 r00, g00, b00, a00;
				Uint8 r01, g01, b01, a01;
				Uint8 r10, g10, b10, a10;
				Uint8 r11, g11, b11, a11;

				SDL_GetRGBA(p00, (*surface)->format, &r00, &g00, &b00, &a00);
				SDL_GetRGBA(p01, (*surface)->format, &r01, &g01, &b01, &a01);
				SDL_GetRGBA(p10, (*surface)->format, &r10, &g10, &b10, &a10);
				SDL_GetRGBA(p11, (*surface)->format, &r11, &g11, &b11, &a11);

				r = (Uint8)((1 - a) * (1 - b) * r00 + a * (1 - b) * r10 +
						(1 - a) * b * r01 + a * b * r11);
				g = (Uint8)((1 - a) * (1 - b) * g00 + a * (1 - b) * g10 +
						(1 - a) * b * g01 + a * b * g11);
				blue = (Uint8)((1 - a) * (1 - b) * b00 + a * (1 - b) * b10 +
						(1 - a) * b * b01 + a * b * b11);
				a_channel = (Uint8)((1 - a) * (1 - b) * a00 + a * (1 - b) * a10 +
						(1 - a) * b * a01 + a * b * a11);

				// Set the pixel value
				rotated_pixels[y * width + x] = SDL_MapRGBA(rotated_surface->format, r, g, blue, a_channel);

			}
		}
	}

	// Unlock the surfaces
	SDL_UnlockSurface(*surface);
	SDL_UnlockSurface(rotated_surface);

	// Free the old surface and update the pointer
	SDL_FreeSurface(*surface);
	*surface = rotated_surface;
}

