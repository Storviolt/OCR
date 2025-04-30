#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <err.h>

// Function to check whether a line is completely white
int is_line_white(SDL_Surface *surface, int y) {
    int width = surface->w;
    for (int x = 0; x < width; x++) {
        Uint32 pixel = ((Uint32*)surface->pixels)[y * width + x];
        if (pixel != SDL_MapRGB(surface->format, 255, 255, 255)) {
            return 0;  // The line contains something other than white
        }
    }
    return 1;  // the line is entirely white
}

// Function to save the image
void save_image(SDL_Surface *surface, const char *filename) {
    if (IMG_SavePNG(surface, filename) != 0) {
        errx(EXIT_FAILURE, "Error saving image: %s", IMG_GetError());
    }
}

//function to load the image
SDL_Surface* load_image(const char *filename) {
    SDL_Surface *loadedSurface = IMG_Load(filename);
    if (!loadedSurface) {
        errx(EXIT_FAILURE, "Error loading image: %s", IMG_GetError());
    }

    // Convert image to 32-bit RGBA format to simplify pixel access
    SDL_Surface *optimizedSurface = 
    SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(loadedSurface);
    if (!optimizedSurface) {
        errx(EXIT_FAILURE, "Error converting image: %s", SDL_GetError());
    }

    return optimizedSurface;
}

// Function to extract and save words from the image
void extract_and_save_words(SDL_Surface *surface, const char *output_dir) {
    int width = surface->w;
    int height = surface->h;

    // Variable to keep track of extracted words
    int word_count = 0;

    // Variables for surrounding words
    int word_start_y = -1;
    int word_end_y = -1;
    
    // Iterate over the image lines
    for (int y = 0; y < height; y++) {
        // If the line is white, it may mark the end of a word
        // if the beginning has already been found.
        if (is_line_white(surface, y)) {
            // If a word has already started (word_start_y is not -1),
            // we mark the end of the word
            if (word_start_y != -1) {
                word_end_y = y - 1;

                // Create a new surface to extract the word
                SDL_Rect word_rect = {0, word_start_y, width,\
                 word_end_y - word_start_y + 1};
                printf("Extracting word from Y = %d to Y = %d\n",\
                 word_start_y, word_end_y);
                SDL_Surface *word_surface = SDL_CreateRGBSurface(0,\
                 word_rect.w, word_rect.h, 32, 0, 0, 0, 0);
                if (!word_surface) {
                    errx(EXIT_FAILURE, "Error creating surface for word extraction: %s",\
                     SDL_GetError());
                }
                SDL_LockSurface(word_surface);
                SDL_LockSurface(surface);
                for (int sy = 0; sy < word_rect.h; sy++) {
                    for (int sx = 0; sx < word_rect.w; sx++) {
                        Uint32 pixel = \
                        ((Uint32*)surface->pixels)[(word_rect.y + sy) *\
                         surface->w + (word_rect.x + sx)];
                        ((Uint32*)word_surface->pixels)[sy *\
                         word_surface->w + sx] = pixel;
                    }
                }
                SDL_UnlockSurface(surface);
                SDL_UnlockSurface(word_surface);

                // Save word image
                char filename[100];
                snprintf(filename, sizeof(filename), "%s/word_%d.png",\
                 output_dir, word_count++);
                save_image(word_surface, filename);

                SDL_FreeSurface(word_surface);

                // Reset for next word
                word_start_y = -1;
                word_end_y = -1;
            }
        } else {
            // If the line is not blank and the word has not yet begun,
            // this marks the beginning of the word.
            if (word_start_y == -1) {
                word_start_y = y;
            }
        }
    }

    // If the last word has not been surrounded by a frame 
    //(if it's at the end of the image), we surround it
    if (word_start_y != -1) {
        word_end_y = height - 1;

        SDL_Rect word_rect = {0, word_start_y, width,\
         word_end_y - word_start_y + 1};
        SDL_Surface *word_surface = SDL_CreateRGBSurface(0,\
         word_rect.w, word_rect.h, 32, 0, 0, 0, 0);
        SDL_LockSurface(word_surface);
        SDL_LockSurface(surface);
        for (int sy = 0; sy < word_rect.h; sy++) {
            for (int sx = 0; sx < word_rect.w; sx++) {
                Uint32 pixel = ((Uint32*)surface->pixels)[(word_rect.y + sy) *\
                 surface->w + (word_rect.x + sx)];
                ((Uint32*)word_surface->pixels)[sy *\
                 word_surface->w + sx] = pixel;
            }
        }
        SDL_UnlockSurface(surface);
        SDL_UnlockSurface(word_surface);

        char filename[100];
        snprintf(filename, sizeof(filename), "%s/word_%d.png",\
         output_dir, word_count++);
        save_image(word_surface, filename);

        SDL_FreeSurface(word_surface);
    }

}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        errx(EXIT_FAILURE,
        "Usage: %s <image path> <output directory>", argv[0]);
    }

    const char *input_file = argv[1];
    const char *output_dir = argv[2];

    // Check if the output directory already exists
    struct stat st;
    if (stat(output_dir, &st) == 0) {
        errx(EXIT_FAILURE,
        "Error: Directory %s already exists.", output_dir);
    } else if (mkdir(output_dir, 0777) != 0) {
        errx(EXIT_FAILURE,
        "Error creating directory %s: %s", output_dir, strerror(errno));
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0 || IMG_Init(IMG_INIT_PNG) == 0) {
        errx(EXIT_FAILURE,
        "SDL initialization error: %s", SDL_GetError());
    }

    SDL_Surface *image = load_image(input_file);
    if (!image) {
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Call the extract_and_save_words function from save_word.c
    extract_and_save_words(image, output_dir);

    SDL_FreeSurface(image);
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
