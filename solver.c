#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void read(char grid[150][150], int size[2], char* file2)
{
    FILE *file;
    file = fopen(file2, "r");

    if (file == NULL) 
    {
        printf("Cannot open file test.txt\n"); 
        return;
    }
    size[0] = 0; //number of rows
    size[1] = 0; //number of columns
    char line[50];


    for(int i = 0; i<50; i++)
    {
        char* positions = fgets(line, sizeof(line), file);
        if(positions == NULL)
        {
            break;
        }
        for(int j = 0; j< strlen(line);j++)
        {
            grid[i][j] = line[j];
        }
        grid[i][strlen(line)] = '\0';
        size[0] = size[0] + 1;
    }
    size[1] = strlen(grid[0]);
    fclose(file);

}

int solver(char grid[150][150], char *word, int positions[4], int size[2])
{
    for(size_t i = 0; i < size[0]; i++)
    {
        for (size_t j = 0; j < size[1]; j++)
        {
            if (grid[i][j] == word[0])
            {
                //right
                int count = 0;
                while(count < strlen(word) && (j+count)< size[1] 
                && grid[i][j+count] == word[count])
                {
                    count = count + 1;
                }
                if (count == strlen(word)) 
                {
                    positions[0] = i;
                    positions[1] = j;
                    positions[2] = i; 
                    positions[3] = j + (count - 1); 
                    return 1;
                }

                //left
                count = 0;
                while(count < strlen(word) && (j-count)< size[1] 
                && grid[i][j-count] == word[count])
                {
                    count = count + 1;
                }
                if (count == strlen(word)) 
                {
                    positions[0] = i;
                    positions[1] = j;
                    positions[2] = i; 
                    positions[3] = j - (count - 1); 
                    return 1;
                }

                //up
                count = 0;
                while (count < strlen(word) && (i - count) >= 0 
                && grid[i - count][j] == word[count]) 
                {
                    count++;
                }
                if (count == strlen(word)) 
                {
                    positions[0] = i;
                    positions[1] = j;
                    positions[2] = i - (count - 1);
                    positions[3] = j;
                    return 1;
                }

                //down
                count = 0;
                while (count < strlen(word) && (i + count) < size[0] 
                && grid[i + count][j] == word[count]) 
                {
                    count++;
                }
                if (count == strlen(word)) 
                {
                    positions[0] = i;
                    positions[1] = j;
                    positions[2] = i + (count - 1);
                    positions[3] = j;
                    return 1;
                }

                //left up
                count = 0;
                while (count < strlen(word) && (i - count) >= 0 
                && (j - count) >= 0 
                && grid[i - count][j - count] == word[count]) 
                {
                    count++;
                }
                if (count == strlen(word)) 
                {
                    positions[0] = i;
                    positions[1] = j;
                    positions[2] = i - (count - 1);
                    positions[3] = j - (count - 1);
                    return 1;
                }

                //right up
                count = 0;
                while (count < strlen(word) && (i - count) >= 0 
                && (j + count) < size[1] 
                && grid[i - count][j + count] == word[count]) 
                {
                    count++;
                }
                if (count == strlen(word)) 
                {
                    positions[0] = i;
                    positions[1] = j;
                    positions[2] = i - (count - 1);
                    positions[3] = j + (count - 1);
                    return 1;
                }

                //left down
                count = 0;
                while (count < strlen(word) && (i + count) < size[0] 
                && (j - count) >= 0 
                && grid[i + count][j - count] == word[count]) 
                {
                    count++;
                }
                if (count == strlen(word)) 
                {
                    positions[0] = i; 
                    positions[1] = j;
                    positions[2] = i + (count - 1);
                    positions[3] = j - (count - 1);
                    return 1;
                }

                //right down
                count = 0;
                while (count < strlen(word) && (i + count) < size[0] 
                && (j + count) < size[1] 
                && grid[i + count][j + count] == word[count]) 
                {
                    count++;
                }
                if (count == strlen(word)) 
                {
                    positions[0] = i;
                    positions[1] = j;
                    positions[2] = i + (count - 1);
                    positions[3] = j + (count - 1);
                    return 1;
                }
            }
            
        }
        
    }
    return 0;
}
