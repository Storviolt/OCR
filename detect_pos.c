#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "painter.h"
#include "load_save.h"
#define BINARIZATION_THRESHOLD 125 // treshold for binarization
void binarize_image(SDL_Surface *surface) {
    SDL_LockSurface(surface);
    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            Uint32 pixel = ((Uint32*)surface->pixels)[y * surface->w + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            // treshold for binarization
            Uint8 binary = (r < BINARIZATION_THRESHOLD) ? 0 : 255;
            ((Uint32*)surface->pixels)[y * surface->w + x] = 
            SDL_MapRGB(surface->format, binary, binary, binary);
        }
    }
    SDL_UnlockSurface(surface);
}

// Function that check whether a column has consecutive white pixels
int SequentialWhitePixel(Uint32 *column, int height, int threshold) {
    int count = 0;
    for (int y = 0; y < height; y++) {
        Uint8 r, g, b;
        SDL_GetRGB(column[y],SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32),
        &r,&g,&b);
        if (r == 255 && g == 255 && b == 255) { //verify if the pixel is white
            count++;
            if (count >= threshold) {
                return 1;// if it has enough consecutive white pixels:return1
            }
        } else {
            count = 0;
        }
    }
    return 0; // else: return 0
}

void detect_grid(SDL_Surface *binary_surface, SDL_Rect *grid_rect,
 int threshold, int expand) {

    int width = binary_surface->w;
    int height = binary_surface->h;

    Uint32 *column_pixels = (Uint32*)calloc(height, sizeof(Uint32));

    // Find the left edge of the grid
    int left_border = -1;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            column_pixels[y]=((Uint32*)binary_surface->pixels)[y * width + x];
        }
        if (!SequentialWhitePixel(column_pixels, height, threshold)) {
            left_border = x; // left edge found
            break;
        }
    }

    // Find the right edge of the grid
    int right_border = -1;
    for (int x = width - 1; x >= 0; x--) {
        for (int y = 0; y < height; y++) {
            column_pixels[y]=((Uint32*)binary_surface->pixels)[y * width + x];
        }
        if (!SequentialWhitePixel(column_pixels, height, threshold)) {
            right_border = x; // right edge found
            break;
        }
    }

    // Find the top edge of the grid
    int top_border = -1;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = ((Uint32*)binary_surface->pixels)[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, binary_surface->format, &r, &g, &b);
            if (r == 0 && g == 0 && b == 0) { // Verify if the pixel is black
                top_border = y; // top edge found
                break;
            }
        }
        if (top_border != -1) {
            break; // exit if the top edge is found
        }
    }

    // Find the bottom edge of the grid
    int bottom_border = -1;
    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = ((Uint32*)binary_surface->pixels)[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, binary_surface->format, &r, &g, &b);
            if (r == 0 && g == 0 && b == 0) { // Verify if the pixel is black
                bottom_border = y; // bottom edge found
                break;
            }
        }
        if (bottom_border != -1) {
            break; // exit if the bottom edge is found
        }
    }

    // Adjust edges to include threshold

    if (left_border != -1) {
        left_border = left_border - expand < 0 ? 0 : left_border - expand;
    }
    if (right_border != -1) {
        right_border = 
        right_border + expand >= width ? width - 1 : right_border + expand;
    }
    if (top_border != -1) {
        top_border = top_border - expand < 0 ? 0 : top_border - expand;
    }
    if (bottom_border != -1) {
        bottom_border = 
        bottom_border + expand >= height ? height - 1 : bottom_border + expand;
    }

    // Check if edges have been found
    if (left_border != -1 && right_border != -1 && top_border != -1 
    && bottom_border != -1) {
        grid_rect->x = left_border;
        grid_rect->y = top_border; // Top edge position
        grid_rect->w = right_border - left_border + 1;
        //  Size based on top and bottom edges
        grid_rect->h = bottom_border - top_border + 1;
        printf("Grid detected : (x: %d, y: %d) - w: %d,\
        h: %d\n", grid_rect->x, grid_rect->y, grid_rect->w, grid_rect->h);
    } else {
        printf("No grid was found.\n");
    }

    free(column_pixels);
}

// Function to detect the word list
void detect_word_list(SDL_Surface *binary_surface, SDL_Rect *word_list_rect,
 SDL_Rect *grid_rect) {
    int width = binary_surface->w;
    int height = binary_surface->h;

    // Initialize edges to maximum values
    int left_border = -1, right_border = -1;
    int top_border = -1, bottom_border = -1;

    // Left edge: first column with a black pixel, excluding the grid area
    for (int x = 0; x < width; x++) {
        // Check if the column is in the grid zone
        if (x >= grid_rect->x && x < grid_rect->x + grid_rect->w) continue; 
        for (int y = 0; y < height; y++) {
            Uint32 pixel = ((Uint32*)binary_surface->pixels)[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, binary_surface->format, &r, &g, &b);
            if (r == 0 && g == 0 && b == 0) { // back pixel
                left_border = x;
                printf("Left edge detected at %d\n", left_border);
                break; // Exit the y loop
            }
        }
        if (left_border != -1) break; // Exit the x loop 
    }

    // Right edge: first column with a black pixel, from right to left, 
    //excluding the grid area.
    for (int x = width - 1; x >= 0; x--) {
        // Check if the column is in the grid zone
        if (x >= grid_rect->x && x < grid_rect->x + grid_rect->w) continue;
        for (int y = 0; y < height; y++) {
            Uint32 pixel = ((Uint32*)binary_surface->pixels)[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, binary_surface->format, &r, &g, &b);
            if (r == 0 && g == 0 && b == 0) { // black pixel
                right_border = x;
                printf("Right edge detected at %d\n", right_border);
                break; // exit y loop
            }
        }
        if (right_border != -1) break; // exit x loop
    }

    // Top edge: search for a line with a black pixel,
    // excluding the column from the grid
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Check if the column is in the grid zone
            if (x >= grid_rect->x && x < grid_rect->x + grid_rect->w)
                continue; // Excludes column from grid
            Uint32 pixel = ((Uint32*)binary_surface->pixels)[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, binary_surface->format, &r, &g, &b);
            if (r == 0 && g == 0 && b == 0) { // black pixel
                top_border = y;
                printf("Top edge detected at %d\n", top_border);
                break; // exit x loop
            }
        }
        if (top_border != -1) break; // exit y loop
    }

    // Bottom edge: search for a line with a black pixel,
    //excluding the column from the grid
    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            // Check if the column is in the grid zone
            if (x >= grid_rect->x && x < grid_rect->x + grid_rect->w)
                continue;  // Excludes the column from the grid
            Uint32 pixel = ((Uint32*)binary_surface->pixels)[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, binary_surface->format, &r, &g, &b);
            if (r == 0 && g == 0 && b == 0) { // black pixel
                bottom_border = y;
                printf("Bottom edge detected at %d\n", bottom_border);
                break; // exit x loop
            }
        }
        if (bottom_border != -1) break; // exit y loop
    }

    // Check if edges have been found
    if (left_border != -1 && right_border != -1 && top_border != -1
    && bottom_border != -1) {
        word_list_rect->x = left_border;
        word_list_rect->y = top_border;
        word_list_rect->w = right_border - left_border + 1;
        // calculate the size of the zone
        word_list_rect->h = bottom_border - top_border + 1;
        printf("Grid detected at: (x: %d, y: %d) - w: %d,\
         h: %d\n", word_list_rect->x, word_list_rect->y,\
          word_list_rect->w, word_list_rect->h);
    } else {
        printf("No grid was found.\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <image path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_image_path = argv[1];

    // Initialization of SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Error when initializing SDL : %s\n",\
         SDL_GetError());
        return EXIT_FAILURE;
    }
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))) {
        fprintf(stderr, "Error when initializing SDL_image : %s\n",\
         IMG_GetError());
        return EXIT_FAILURE;
    }

    // Load image
    SDL_Surface *image_surface = load_image(input_image_path);
    if (!image_surface) {
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Binarize the image
    binarize_image(image_surface);
    save_image(image_surface, "output_binary.png");

    // Detect the grid
    SDL_Rect grid_rect;
    detect_grid(image_surface, &grid_rect, image_surface->h / 8, 5);
    // use: height / 8 as treshold and expand to 5 pixels

    // Create a new surface to draw the detected grid
    SDL_Surface *grid_surface = SDL_CreateRGBSurface(0,
     grid_rect.w, grid_rect.h, 32, 0, 0, 0, 0);

    SDL_LockSurface(grid_surface);
    SDL_LockSurface(image_surface);
    for (int y = 0; y < grid_rect.h; y++) {
        for (int x = 0; x < grid_rect.w; x++) {
            Uint32 pixel = ((Uint32*)image_surface->pixels)
            [(grid_rect.y + y) * image_surface->w + (grid_rect.x + x)];
            ((Uint32*)grid_surface->pixels)[y * grid_surface->w + x] = pixel;
        }
    }
    SDL_UnlockSurface(image_surface);
    SDL_UnlockSurface(grid_surface);

    // Save the grid
    save_image(grid_surface, "output_grid.png");

    // detect area of the list of word
    SDL_Rect word_list_rect;
    detect_word_list(image_surface, &word_list_rect, &grid_rect);

    // Create a new surface to draw the detected word list
    SDL_Surface *word_list_surface = 
    SDL_CreateRGBSurface(0,word_list_rect.w, word_list_rect.h, 32, 0, 0, 0, 0);
    SDL_LockSurface(word_list_surface);

    SDL_LockSurface(image_surface);
    for (int y = 0; y < word_list_rect.h; y++) {
        for (int x = 0; x < word_list_rect.w; x++) {
            Uint32 pixel = ((Uint32*)image_surface->pixels)
            [(word_list_rect.y + y) * image_surface->w+(word_list_rect.x + x)];
            ((Uint32*)word_list_surface->pixels)[y*word_list_surface->w+x] =
             pixel;
        }
    }
    SDL_UnlockSurface(image_surface);
    SDL_UnlockSurface(word_list_surface);

    // Save the image  and the list of word
    save_image(word_list_surface, "output_word_list.png");

    // free
    SDL_FreeSurface(image_surface);
    SDL_FreeSurface(grid_surface);
    SDL_FreeSurface(word_list_surface);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
