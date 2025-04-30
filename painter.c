#include "painter.h"
Uint32 rcolor(SDL_PixelFormat *format, int i) {
    if (!format) {
        return 0; // Returns a black color if the format is invalid
    }

    // array of predefined RGB colors
    Uint8 colors[][3] = {
        {0, 0, 255},    // blue
        {255, 0, 0},    // red
        {0, 255, 0},    // green
        {255, 255, 0},  // yellow
        {128, 0, 128},  // purple
        {255, 165, 0}   // orange
    };


    // Random selection of an index in the array
    int index = i;

    //Extract the RGB components of the selected color
    Uint8 red = colors[index][0];
    Uint8 green = colors[index][1];
    Uint8 blue = colors[index][2];

    // Returns the color as a surface-compatible Uint32
    return SDL_MapRGB(format, red, green, blue);
}

void draw_line(SDL_Surface *surface, int x, int y, int x2, int y2, int i) {
    if (!surface) {
        return; // the surface must be valid
    }

    int width = surface->w;
    int height = surface->h;

    if (SDL_MUSTLOCK(surface)) {
        SDL_LockSurface(surface);
    }

    int dist_x = abs(x2 - x);
    int dist_y = -abs(y2 - y);

    int add_x = -1;
    int add_y = -1;

    if (x < x2) {
        // If the start point is to the left of the end point,
        // we move to the right
        add_x = 1;
    }

    if (y < y2) {
        add_y = 1; // If the start point is above the end point, we descend
    }

    int err = dist_x + dist_y, e2;

    while (1) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            ((Uint32 *)surface->pixels)[y * width + x] =\
             rcolor((surface->format),i);
        }

        if (x == x2 && y == y2) {
            break;
        }

        e2 = 2 * err;
        if (e2 >= dist_y) {
            err += dist_y;
            x += add_x;
        }
        if (e2 <= dist_x) {
            err += dist_x;
            y += add_y;
        }
    }

    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }
}

void draw(SDL_Surface *surface, int x, int y, int x2, int y2, int size) {
    if (!surface) {
        return; // the surface must be valid
    }

    int width = surface->w;
    int height = surface->h;

    // Make sure the surface can be modified
    if (SDL_MUSTLOCK(surface)) {
        SDL_LockSurface(surface);
    }

    // Swap points for a logical order
    if (y2 < y && x2 < x) {
        int temp = y2;
        y2 = y;
        y = temp;
        temp = x2;
        x2 = x;
        x = temp;
    }
    else if ((x > x2 && y < y2))
    {
        int temp = y2;
        y2 = y;
        y = temp;
        temp = x2;
        x2 = x;
        x = temp;
    }

    int c = rand()%6;

    // If the points are aligned horizontally
    if (y == y2) {
        if (x2 < x)
        {
            int temp = x2; 
            x2 = x; 
            x = temp;
        }
        // Upper left edge
        draw_line(surface, x, y + size/2, x + size/2, y, c);
        // Lower left edge
        draw_line(surface, x, y + size/2, x + size/2, y + size, c); 
        // Top line
        draw_line(surface, x + size/2, y, x2 + size/2, y2, c);
        // Bottom line
        draw_line(surface, x + size/2, y + size, x2 + size/2, y2 + size, c);
        // Upper right edge
        draw_line(surface, x2 + size, y2 + size/2, x2 + size/2, y2, c);
        // Bottom right edge
        draw_line(surface, x2 + size, y2 + size/2, x2 + size/2, y2 + size, c);
    }

    // If the points are vertically aligned
    else if (x == x2) {
        // Top left edge
        draw_line(surface, x + size/2, y, x, y + size/2, c);
        // top right edge
        draw_line(surface, x + size/2, y, x + size, y + size/2, c);
        // left line
        draw_line(surface, x, y + size/2, x2, y2 + size/2, c);
        // right line
        draw_line(surface, x + size, y + size/2, x2 + size, y2 + size/2, c);
        // left bottom edge
        draw_line(surface, x2 + size/2, y2 + size, x2, y2 + size/2, c);
        // right bottom edge
        draw_line(surface, x2 + size/2, y2 + size, x2 + size, y2 + size/2, c);
    }

    // If the points form a diagonal (case 1)
    else if ((x > x2 && y < y2) || (x < x2 && y > y2)) {
        //bottom left edge
        draw_line(surface, x, y + size, x + size/2, y + size, c);
        // top left edge
        draw_line(surface, x, y + size, x, y + size/2, c);     
        // top diagonal
        draw_line(surface, x, y + size/2, x2 + size/2, y2, c);
        // bottom diagonal
        draw_line(surface, x + size/2, y + size, x2 + size, y2 + size/2, c);
        // top right edge
        draw_line(surface, x2 + size, y2, x2 + size, y2 + size/2, c);
        // bottom right edge
        draw_line(surface, x2 + size, y2, x2 + size/2, y2, c);
    }

    // If the points form a diagonal (case 2)
    else {
        //top left edge
        draw_line(surface, x, y, x + size/2, y, c);
        //bot left edge
        draw_line(surface, x, y, x, y + size/2, c);
        // top diagonal
        draw_line(surface, x + size/2, y, x2+size, y2 + size/2, c);
        // bottom diagonal
        draw_line(surface, x, y + size/2, x2 + size/2, y2 + size, c);

        // top right edge
        draw_line(surface, x2 + size, y2 + size, x2 + size, y2 + size/2, c);
        // bottom right edge
        draw_line(surface, x2 + size, y2 + size, x2 + size/2, y2 + size, c);
    }

    // unlocking the surface
    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }
}


void detect(SDL_Surface *surface, int x, int y, int x2, int y2, int size)
{
    draw(surface, x, y, x2, y2, size);
}

