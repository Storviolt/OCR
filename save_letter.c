#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <err.h>

#include "load_save.h"


typedef struct {
    int x;
    int y;
} Coordinate;

Coordinate get_coordinate(Coordinate *coordinates, \
int rows, int cols, int i, int j) {

    // Calculate index based on row and column indices
    int index = i * cols + j;

    // verify if the index is valid
    if (index >= rows * cols) {
        printf("Index (%d, %d) out of bounds\n", i, j);
        // Or return a default value if you prefer not to quit the program
        exit(EXIT_FAILURE);
    }

    return coordinates[index];  // Return struct Coordinate to this index
}


void print_coordinates(Coordinate *coordinates, int rows, int cols) {
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            Coordinate coord = get_coordinate(coordinates,rows,cols, row, col);
            printf("letter[%d][%d] = ( %d , %d )\n",row,col, coord.x, coord.y);
        }
    }
}


void update_rows_and_cols(Coordinate *coordinates, int letter_count,\
 int *rows, int *cols) {
    *cols = 1; // Start with at least one column
    int current_row_y = coordinates[0].y; // First line initialization
    int threshold = 5; // Variation considered as a new line

    // Iterate the letters to determine the number of columns
    for (int i = 1; i < letter_count; i++) {
        if (abs(coordinates[i].y - current_row_y) > threshold) {
            // new line detected
            break;
        } else {
            // Add column to current row
            (*cols)++;
        }
    }

    // calculate lines
    *rows = (letter_count + *cols - 1) / *cols; // Division rounded up
    printf("rows = %d , col = %d\n", *rows, *cols);
}

Coordinate* extract_and_save_letters(SDL_Surface *surface,\
 const char *output_dir, int *letter_count, int *rows, int *cols) {

    int width = surface->w;
    int height = surface->h;

    // Array to mark visited pixels
    int *visited = (int*)calloc(width * height, sizeof(int));
    if (!visited) {
        errx(EXIT_FAILURE, "Memory allocation error");
    }

    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    *letter_count = 0;

    // Assume a fixed line height for simplicity (can be adjusted as needed)
    int line_height = 50; // Adjust based on your grid
    int col_width = 50;   // Adjust based on your grid

    // Dynamic array for coordinates
    int max_letters = 100; // Initial size for the array
    Coordinate *coordinates = \
    (Coordinate*)malloc(max_letters * sizeof(Coordinate));

    if (!coordinates) {
        errx(EXIT_FAILURE, "Memory allocation error");
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // If the pixel is black and not visited
            if (((Uint32*)surface->pixels)[y * width + x] ==
                SDL_MapRGB(surface->format, 0, 0, 0) \
                && !visited[y * width + x]) {
                // Initialize bounding box coordinates
                int min_x = x, min_y = y, max_x = x, max_y = y;

                // Stack for depth-first search
                SDL_Point stack[width * height];
                int stack_size = 0;

                // Add current point to the stack
                stack[stack_size++] = (SDL_Point){x, y};

                while (stack_size > 0) {
                    SDL_Point p = stack[--stack_size];

                    // Check bounds
                    if (p.x < 0 || p.x >= width || p.y < 0 || p.y >= height)
                        continue;

                    // If the pixel is black and not visited
                    if (((Uint32*)surface->pixels)[p.y * width + p.x] ==
                        SDL_MapRGB(surface->format, 0, 0, 0) &&
                        !visited[p.y * width + p.x]) {
                        visited[p.y * width + p.x] = 1;
                        min_x = (p.x < min_x) ? p.x : min_x;
                        min_y = (p.y < min_y) ? p.y : min_y;
                        max_x = (p.x > max_x) ? p.x : max_x;
                        max_y = (p.y > max_y) ? p.y : max_y;

                        // Add neighbors to stack
                        for (int i = 0; i < 4; i++) {
                            stack[stack_size++] = 
                                (SDL_Point){p.x + directions[i][0], 
                                            p.y + directions[i][1]};
                        }
                    }
                }

                // Define the bounding box for the detected component
                SDL_Rect rect = {min_x, min_y,\
                 max_x - min_x + 1, max_y - min_y + 1};

                // Save the letter as an individual image
                SDL_Surface *letter_surface = 
                    SDL_CreateRGBSurface(0, rect.w, rect.h, 32, 0, 0, 0, 0);

                SDL_Rect dst_rect = {0, 0, rect.w, rect.h};
                SDL_BlitSurface(surface, &rect, letter_surface, &dst_rect);

                char filename[200];
                snprintf(filename, sizeof(filename), 
                         "%s/letter_%dx%d.png", output_dir, \
                         min_y / line_height, min_x / col_width);

                save_image(letter_surface, filename);
                SDL_FreeSurface(letter_surface);

                // Store the top-left corner coordinate in the array
                if (*letter_count >= max_letters) {
                    max_letters *= 2;
                    coordinates = \
                    (Coordinate*)realloc(coordinates,\
                     max_letters * sizeof(Coordinate));
                    if (!coordinates) {
                        errx(EXIT_FAILURE, "Memory reallocation error");
                    }
                }

                coordinates[*letter_count].x = min_x;
                coordinates[*letter_count].y = min_y;
                (*letter_count)++;
            }
        }
    }

    // Update rows and cols based on the coordinates
    update_rows_and_cols(coordinates, *letter_count, rows, cols);

    free(visited);
    return coordinates;
}

// verify if a line is entirely white pixels
int is_line_white(SDL_Surface *surface, int y) {
    int width = surface->w;
    for (int x = 0; x < width; x++) {
        Uint32 pixel = ((Uint32*)surface->pixels)[y * width + x];
        // If a pixel is not white, the line is not completely white
        if (pixel != SDL_MapRGB(surface->format, 255, 255, 255)) {
            return 0;  // The line contains something other than white
        }
    }
    return 1;  // The line is completely white
}

// Function to extract and save words from the image ( word list )
void extract_and_save_words(const char *image_path,\
 const char *output_dir) {
    // load image
    SDL_Surface *surface = load_image(image_path);
    if (!surface) {
        errx(EXIT_FAILURE, "Error loading image: %s", image_path);
    }

    int width = surface->w;
    int height = surface->h;

    // Create the list_of_word/ directory if it doesn't exist
    char list_of_word_dir[1024];
    snprintf(list_of_word_dir, sizeof(list_of_word_dir), "m%s", output_dir);

    struct stat st;
    if (stat(list_of_word_dir, &st) != 0) {
        if (mkdir(list_of_word_dir, 0777) != 0) {
            errx(EXIT_FAILURE, "Error creating directory %s: %s",\
             list_of_word_dir, strerror(errno));
        }
    }

    // Variables to keep track of the current word
    int word_count = 0;
    int word_start_x = -1;
    int word_end_x = -1;

    // Detect words line by line
    for (int y = 0; y < height; y++) {
         // verify if the line is white
        int is_white_line = is_line_white(surface, y);

        // if the line is white, this means that a previous word has ended
        if (is_white_line && word_start_x != -1 && word_end_x != -1) {
            // save the word
            SDL_Rect word_rect = {word_start_x, y - 1,\
             word_end_x - word_start_x + 1, 1};

            // create a surface for the extracted word
            SDL_Surface *word_surface = SDL_CreateRGBSurface(0, word_rect.w,\
             word_rect.h, 32, 0, 0, 0, 0);
            SDL_Rect dst_rect = {0, 0, word_rect.w, word_rect.h};
            SDL_BlitSurface(surface, &word_rect, word_surface, &dst_rect);

            // save the image of the word with its number
            char filename[100];
            snprintf(filename, sizeof(filename), "%s/word_%d.png", output_dir,\
             word_count++);
            save_image(word_surface, filename);

            SDL_FreeSurface(word_surface);

            // reset variables for the next word
            word_start_x = -1;
            word_end_x = -1;
        }
        // iterate over pixels of the line ( in x)
        else {
            for (int x = 0; x < width; x++) {
                Uint32 pixel = ((Uint32*)surface->pixels)[y * width + x];

                // If the pixel is black, it's part of the word
                if (pixel == SDL_MapRGB(surface->format, 0, 0, 0)) {
                    if (word_start_x == -1) {
                        word_start_x = x;  // start of the word
                    }
                    word_end_x = x;  // end of the word
                }
            }
        }
    }

    // if a word is still in progress at the end of the image, it is saved
    if (word_start_x != -1 && word_end_x != -1) {
        SDL_Rect word_rect = {word_start_x, height - 1,\
         word_end_x - word_start_x + 1, 1};

        SDL_Surface *word_surface = SDL_CreateRGBSurface(0, word_rect.w,\
         word_rect.h, 32, 0, 0, 0, 0);
        SDL_Rect dst_rect = {0, 0, word_rect.w, word_rect.h};
        SDL_BlitSurface(surface, &word_rect, word_surface, &dst_rect);

        char filename[100];
        snprintf(filename, sizeof(filename), "%s/word_%d.png",\
         output_dir, word_count++);
        save_image(word_surface, filename);

        SDL_FreeSurface(word_surface);
    }

    // Free the image surface
    SDL_FreeSurface(surface);
}



/*

int main(int argc, char *argv[]) {
    if (argc < 3) {
        errx(EXIT_FAILURE,
        "Usage: %s <image path> <output directory>", argv[0]);
    }

    const char *input_file = argv[1];
    const char *output_dir = argv[2];
    const char *output_file = "output_image.png";

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

    int letter_count = 0;
    int rows = 0, cols = 0;

    // Extraire les lettres et enregistrer les images
    Coordinate *coordinates = extract_and_save_letters(image, output_dir, &letter_count, &rows, &cols);
    // Afficher les coordonnées
    print_coordinates(coordinates, rows, cols);
    // Libérer la mémoire allouée
    free(coordinates);

    // extract_and_save_letters(image, output_dir);
    // save_image(image, output_file);

    SDL_FreeSurface(image);
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
*/
