#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "solver.h"

void read_words(char* filename, char words[100][50], int* word_count) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    *word_count = 0;
    char line[50];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0'; // Remove newline character
        strcpy(words[*word_count], line);
        (*word_count)++;
    }
    fclose(file);
}
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <grid_file> <words_file>\n", argv[0]);
        return 1;
    }

    char grid[150][150]; // Grid as a matrix
    int size[2];         // Grid size
    read(grid, size, argv[1]);

    char words[100][50]; // Words to search
    int word_count = 0;  // Number of words
    read_words(argv[2], words, &word_count);

    // Open the output file to write results
    FILE *output_file = fopen("coordinates.txt", "w");
    if (output_file == NULL) {
        printf("Cannot open output file for writing\n");
        return 1;
    }

    for (int k = 0; k < word_count; k++) {
        char word[50];
        strcpy(word, words[k]);

        // Convert the word to uppercase
        for (int i = 0; word[i] != '\0'; i++) {
            word[i] = toupper(word[i]);
        }

        int positions[4];
        int res = solver(grid, word, positions, size);

        if (res == 1) {
            int x1 = positions[1];
            int y1 = positions[0];
            int x2 = positions[3];
            int y2 = positions[2];

            // Write the coordinates to the output file (no print to console)
            fprintf(output_file, "(%i,%i)-(%i,%i)\n", x1, y1, x2, y2);
        } else {
            // If the word is not found, write "not found"
            fprintf(output_file, "Not found\n");
        }
    }

    fclose(output_file); // Close the output file
    return 0;
}
