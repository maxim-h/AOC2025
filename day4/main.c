#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


size_t swipe(char* scene, char* next_scene, size_t height, size_t width, size_t* sum) {
    size_t n_removed = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            size_t n_neighbours = 0;

            if (*(scene + i*width + j) != '@') continue;

            for (int k=-1; k <= 1; k++) {
                for (int l = -1; l<=1; l++) {
                    if (k == 0 && l == 0) continue;
                    if ((i+k) < 0 || (i+k) > height-1) continue;
                    if ((j+l) < 0 || (j+l) > width-1) continue;

                    if (*(scene + (i+k)*width + j+l) == '@') {
                        n_neighbours++;
                    }
                }
            }

            if (n_neighbours < 4) {
                (*sum)++;
                n_removed++;
                *(next_scene + i*width + j) = '.';
            }
        }
    }
    // printf("Removed: %ld\n", n_removed);
    return n_removed;
}

int main() {
    char* filename = "./input.txt";
    char strbuf[1024] = {0};
    size_t height = 0;
    size_t width = 0;
    size_t sum = 0;
    FILE* f = fopen(filename, "r");

    while (true) {
        char* line = fgets(strbuf, 1024, f);
        if (height == 0) {
            size_t len = strlen(line);
            width = len - 1;
        }
        if (line == NULL) break;
        height++;
    }
    fclose(f);

    f = fopen(filename, "r");

    char* scene = malloc(width*height);
    char* next_scene = malloc(width*height);


    for (int i = 0; i < height; i++) {
        char* line = fgets(strbuf, 1024, f);
        size_t l = strlen(line);
        l--;
        line[l] = '\0';
        memcpy(scene + i*width, line, l);
    }

    memcpy(next_scene, scene, width*height);

    while (true) {
        size_t n_removed = swipe(scene, next_scene, height, width, &sum);

        if (n_removed == 0) break;

        scene = next_scene;

    }


//     for (int i = 0; i < height; i++) {
//         char* pb = malloc(width+1);
//         memset(pb, '\0', width+1);

//         memcpy(pb, scene+ i*width, width);
//         printf("%s\n", pb);
//     }

    printf("%ld\n", sum);

    fclose(f);
    return 0;
}
