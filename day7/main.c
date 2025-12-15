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

typedef struct {
    struct Node *child[2];
    size_t indegree;
} Node;

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
    static uint8_t beams[(160 + 7)/8];
    DA* timelines = da_init(1000);

    FILE* f = fopen(filename, "r");

    // Handle first line separately
    // Identify the starting position
    // Find the line length to allocate the index buffer
    char* line = fgets(strbuf, sizeof(strbuf), f);

    for (size_t i = 0; ; i++) {
        if (*(line + i) == 'S') {
            bitset_insert(beams, i);
            da_append(timelines, &beams, 1);
            break;
        }
    }
    // printf("%s", line);

    size_t line_n = 1;
    while (true) {
        char* line = fgets(strbuf, sizeof(strbuf), f);
        if (line == NULL) break;
        line_n++;
        printf("Parsing line %ld\n", line_n);
        for (size_t i=0; ; i++) {
            if (*(line + i) == '\n') break;

            if (*(line + i) == '^') {
                // *(line + i - 1) = '|';
                // *(line + i + 1) = '|';
                printf("Found splitter at [%ld].", i);
                size_t n_timelines_cur = timelines->count;
                for (size_t j = 0; j < n_timelines_cur; j++) {
                    uint8_t* timeline_beams = timelines->elements[j];
                    if (bitset_test(timeline_beams, i)) {
                        uint8_t* new_timeline = malloc(sizeof(*timeline_beams));
                        memcpy(new_timeline, timeline_beams, sizeof(*timeline_beams));
                        da_append(timelines, new_timeline, 1);
                        bitset_insert(new_timeline, i + 1); // make new timeline to the right
                        bitset_insert(timeline_beams, i - 1); // existing timeline always on the left
                        bitset_delete(timeline_beams, i);
                        bitset_delete(new_timeline, i);
                        sum++;
                    }
                }
                printf("\n");
            }
            // if (bitset_test(beams, i)) {
            //     *(line + i) = '|';
            // }
        }
        // printf("%s", line);
    }

    fclose(f);
    printf("%ld\n", sum);
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
