#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "../lib/DynArray.h"


static inline void bitset_insert(uint8_t* bits, int x) {
    bits[x >> 3] |= (uint8_t)(1u << (x & 7));
}

static inline bool bitset_test(const  uint8_t* bits, int x) {
    return bits[x >> 3] & (uint8_t)(1u << (x & 7));
}

static inline void bitset_delete(uint8_t* bits, int x) {
    bits[x >> 3] &= (uint8_t)~(1u << (x & 7));
}

// typedef struct {
//     struct Node *child[2];
//     size_t indegree;
// } Node;

typedef struct {
    size_t index;
    size_t n_timelines;
} Beam;

int solution1(char* filename) {
    size_t sum = 0;
    size_t n_splits = 0;
    char strbuf[4096] = {0};
    static uint8_t beams[(4096 + 7)/8];

    FILE* f = fopen(filename, "r");

    // Handle first line separately
    // Identify the starting position
    // Find the line length to allocate the index buffer
    char* line = fgets(strbuf, sizeof(strbuf), f);

    for (size_t i = 0; ; i++) {
        if (*(line + i) == 'S') {
            bitset_insert(beams, i);
            break;
        }
    }
    // printf("%s", line);

    while (true) {
        char* line = fgets(strbuf, sizeof(strbuf), f);
        if (line == NULL) break;
        for (size_t i=0; ; i++) {
            if (*(line + i) == '\n') break;

            if (*(line + i) == '^' && bitset_test(beams, i)) {
                *(line + i - 1) = '|';
                *(line + i + 1) = '|';
                bitset_insert(beams, i + 1);
                bitset_insert(beams, i - 1);
                bitset_delete(beams, i);
                sum++;
            }
            if (bitset_test(beams, i)) {
                *(line + i) = '|';
            }
        }
        // printf("%s", line);
    }

    fclose(f);
    printf("%ld\n", sum);
    return 0;
}

int solution2(char* filename) {
    size_t sum = 1;
    size_t n_splits = 0;
    char strbuf[4096] = {0};
    // static uint8_t beams[(160 + 7)/8];
    DA* beams = da_init(1000);

    FILE* f = fopen(filename, "r");

    // Handle first line separately
    // Identify the starting position
    // Find the line length to allocate the index buffer
    char* line = fgets(strbuf, sizeof(strbuf), f);

    for (size_t i = 0; ; i++) {
        if (*(line + i) == 'S') {
            Beam* beam = malloc(sizeof(Beam));
            beam->index = i;
            beam->n_timelines = 1;
            da_append(beams, beam, 1);
            break;
        }
    }
    // printf("%s", line);

    size_t line_n = 1;
    while (true) {
        char* line = fgets(strbuf, sizeof(strbuf), f);
        if (line == NULL) break;
        line_n++;
        DA* new_beams = da_init(beams->count);
        // printf("Parsing line %ld\n", line_n);
        // iteration can be inverted to only check chars at beam positions
        for (size_t i=0; ; i++) {
            if (*(line + i) == '\n') {
                // DA* old_beams = beams;
                beams = new_beams;
                // da_free(old_beams); //might be freeing stack pointers
                break;
            }

            if (*(line + i) == '^') {
                size_t in_paths = 0;
                bool split = false;

                for (size_t b=0; b < beams->count; b++) {
                    Beam* beam = beams->elements[b];
                    // printf("%ld: %ld", i, beam->index);
                    if (beam->index == i) {
                        split = true;
                        in_paths += beam->n_timelines;
                    }
                }

                if (split) {
                    // printf("Splitting at %ld with %ld paths\n", i, in_paths);
                    Beam* left = malloc(sizeof(Beam));
                    left->index = i - 1;
                    left->n_timelines = in_paths;
                    Beam* right = malloc(sizeof(Beam));
                    right->index = i + 1;
                    right->n_timelines = in_paths;
                    da_append(new_beams, left, 1);
                    da_append(new_beams, right, 1);
                    split = false;
                }
            } else {
                for (size_t b=0; b < beams->count; b++) {
                    Beam* beam = beams->elements[b];
                    if (beam->index == i) {
                        da_append(new_beams, beam, 1);
                    }
                }
            }
        }
    }

    for (size_t i=0; i < beams->count; i++) {
        Beam* beam = beams->elements[i];
        sum += beam->n_timelines;
    }

    fclose(f);
    // I still don't know why it's -1 ¯\_(ツ)_/¯
    printf("%ld\n", sum - 1);
    return 0;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Exactly one command-line argument expected - input filename.\n");
        printf("%d provided.\n", argc - 1);
        return 1;
    }

    char* filename = argv[1];
    return solution2(filename);
}
