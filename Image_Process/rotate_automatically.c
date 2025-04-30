#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>
#include "rotate_automatically.h"
#include "rotate_manually.h" // To use the `apply_rotation` function.

double detect_rotation_angle(SDL_Surface *surface) {
    int w = surface->w;
    int h = surface->h;
    Uint32 *pixels = (Uint32 *)surface->pixels;

    double sum_angles = 0;
    int count = 0;

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            Uint32 p1 = pixels[y * w + x];         
            Uint32 p2 = pixels[y * w + (x + 1)];   
            Uint32 p3 = pixels[(y + 1) * w + x];   

            Uint8 r1, g1, b1, r2, g2, b2, r3, g3, b3;
            SDL_GetRGB(p1, surface->format, &r1, &g1, &b1);
            SDL_GetRGB(p2, surface->format, &r2, &g2, &b2);
            SDL_GetRGB(p3, surface->format, &r3, &g3, &b3);

            int luminance1 = (int)(0.3 * r1 + 0.59 * g1 + 0.11 * b1);
            int luminance2 = (int)(0.3 * r2 + 0.59 * g2 + 0.11 * b2);
            int luminance3 = (int)(0.3 * r3 + 0.59 * g3 + 0.11 * b3);

            int dx = luminance2 - luminance1;  
            int dy = luminance3 - luminance1;  

            if (dx != 0 || dy != 0) {
                double angle = atan2((double)dy, (double)dx);
                sum_angles += angle;
                count++;
            }
        }
    }

    double average_angle = (count > 0) ? (sum_angles / count) : 0.0;
    double angle_degrees = average_angle * (180.0 / M_PI);

    if (angle_degrees < -90) angle_degrees += 180;
    else if (angle_degrees > 90) angle_degrees -= 180;

    return angle_degrees;
}

void apply_rotate_automatically(SDL_Surface **surface) {
    if (!(*surface)) {
        fprintf(stderr, "Error: No surface provided for automatic rotation.\n");
        return;
    }

    double angle = detect_rotation_angle(*surface);
    if (fabs(angle) >= 20) {
        printf("Detected rotation angle: %f degrees. Correcting...\n", angle);
        apply_rotation(surface, -angle);  // Correct the rotation using the function from `rotate_manually.c`.
    } else {
        printf("No significant rotation detected. Skipping rotation.\n");
    }
}

