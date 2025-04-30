#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <err.h>
#include "save_letter.h"
#include "load_save.h"
#include "painter.h"
int parse_line(const char *line, int *start1, int *end1,\
 int *start2, int *end2) {
    return sscanf(line, "(%d,%d)-(%d,%d)", start1, end1, \
    start2, end2);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_file = argv[1];
    const char *image_file = "output_grid.png";
    const char *output_file = "final_output.png";
    const char *output_dir = "LETTER";

    if (SDL_Init(SDL_INIT_VIDEO) < 0 || IMG_Init(IMG_INIT_PNG) == 0) {
        errx(EXIT_FAILURE, "Error when initializing SDL : %s", SDL_GetError());
    }

    SDL_Surface *image = load_image(image_file);
    if (!image) {
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Surface *grid_surface = \
    SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGBA32, 0);
    if (!grid_surface) {
        errx(EXIT_FAILURE, "Image area conversion error : %s", SDL_GetError());
    }

    // Check if the directory exists, if not create it
    struct stat st = {0};
    if (stat(output_dir, &st) == -1) {
        // Create directory with 0700 permissions
        if (mkdir(output_dir, 0700) == -1) { 
            errx(EXIT_FAILURE, "Directory creation error %s: %s",\
             output_dir, strerror(errno));
        }
    }

    int letter_count = 0, rows = 0, cols = 0;
    Coordinate *coordinates = extract_and_save_letters(image, output_dir, \
    &letter_count, &rows, &cols);
    if (letter_count < 2) {
        errx(EXIT_FAILURE, "Not enough letters detected in the image.");
    }

    FILE *file = fopen(input_file, "r");
    if (!file) {
        perror("Error opening input file");
        SDL_FreeSurface(image);
        SDL_FreeSurface(grid_surface);
        free(coordinates);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        int start1, end1, start2, end2;
        if (parse_line(line, &start1, &end1, &start2, &end2) != 4) {
            fprintf(stderr, "Skipping invalid line: %s", line);
            continue;
        }

        int x1 = get_coordinate(coordinates, rows, cols, start1, end1).x;
        int y1 = get_coordinate(coordinates, rows, cols, start1, end1).y;
        int x2 = get_coordinate(coordinates, rows, cols, start2, end2).x;
        int y2 = get_coordinate(coordinates, rows, cols, start2, end2).y;
        printf("x1 = %d, y1 = %d , x2 = %d, y2 = %d\n",x1,y1, x2, y2);
        int size = 40;
        detect(grid_surface, x1, y1, x2, y2, size);
    }

    fclose(file);

    save_image(grid_surface, output_file);
    printf("Final output saved to %s\n", output_file);

    SDL_FreeSurface(image);
    SDL_FreeSurface(grid_surface);
    free(coordinates);
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}

