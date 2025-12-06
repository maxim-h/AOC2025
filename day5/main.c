#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

static char range_separator = '-';

typedef struct {
    size_t capacity;
    size_t count;
    void** elements;
} DA;

DA* da_init(size_t initial_capacity) {
    DA* result = malloc(sizeof(DA));
    void* elements = malloc(initial_capacity*sizeof(void *));
    DA da = {
        initial_capacity,
        0,
        elements
    };
    *result = da;

    return result;
}

// Very shitty and slow. Not very N+2
int da_free(DA* da) {
    for (size_t i=0; i < da->count; i++) {
        free(da->elements[i]);
    }
    free(da->elements);
    free(da);

    return 0;
}

int da_append(DA* da, void* new, size_t size) {
    if ((da->count + size) > da->capacity) {
        void* new_elements = realloc(da->elements, 2*(da->capacity)*sizeof(void *));
        if (new_elements == NULL) return 1;
        da->elements = new_elements;
        da->capacity = 2*(da->capacity);
    }

    da->elements[da->count] = new;
    da->count += size;

    return 0;
}

void* da_element(DA* da, size_t i) {
    if (i > da->count) return NULL;
    return da->elements[i];

}

typedef struct {
    size_t start;
    size_t end;
} Range;

int parse_range(DA* ranges, char* line, size_t len) {
    DA* to_be_removed = da_init(10);
    char* current_line = malloc((len+1)*sizeof(char));
    memcpy(current_line, line, len+1);
    char* startp = strsep(&current_line, &range_separator);

    size_t start = strtol(startp, NULL, 10);
    size_t end = strtol(current_line, NULL, 10);

    Range r = {start, end};

    // we use uintptr to hack our DA to store indices instead of pointers
    for (uintptr_t i=0; i < ranges->count; i++) {
        Range* current_range = da_element(ranges, i);
        if (current_range == NULL) {
            printf("Couldn't not access element %ld.\n", i);
            return 1;
        }

        // if some existing range overlaps our new range we will potentially
        // extend the new range to include the old one and mark the old one
        // for deletion
        if (r.start <= current_range->start) {
            if (r.end <= current_range->end && r.end >= (current_range->start - 1)) {
                r.end = current_range->end;
                da_append(to_be_removed, (void *)i, 1);
                continue;
            } else if (r.end > current_range->end) {
                da_append(to_be_removed, (void *)i, 1);
                continue;
            } else if (r.start == current_range->start && r.end <= current_range->end) {
                return 0;
            }
        } else {
            if (r.end >= current_range->end && r.start <= (current_range->end + 1)) {
                r.start = current_range->start;
                da_append(to_be_removed, (void *)i, 1);
                continue;
            } else if (r.end <= current_range->end && r.start < current_range->end) {
                return 0;
            }
        }
    }


    Range* new_range = malloc(sizeof(Range));
    *new_range = r;
    da_append(ranges, new_range, 1);

    // If some ranges need removal we will swap the old ranges array with a
    // new one that doesn't include them
    if (to_be_removed->count > 0) {
        // not sure what the resulting array size should be
        // currently we fit it exactly, but that may cause more reallocations
        // while we're still parsing ranges
        DA* new_ranges = da_init(ranges->capacity - to_be_removed->count);
        bool skip_range = false;
        for (uintptr_t i = 0; i < ranges->count; i++) {
            for (size_t j = 0; j < to_be_removed->count; j++) {
                if (i == (uintptr_t)to_be_removed->elements[j]) {
                    free(da_element(ranges, i));
                    skip_range = true;
                    break;
                }
            }
            if (skip_range) {
                skip_range = false;
                continue;
            }
            da_append(new_ranges, ranges->elements[i], 1);
        }

        DA old_ranges = *ranges;
        *ranges = *new_ranges;
        free(old_ranges.elements); // don't know why this frees unallocated memory
    }

    // da_free(to_be_removed);
    return 0;
}

int parse_item(DA* items, char* line, size_t size) {
    size_t item = strtol(line, NULL, 10);

    size_t* itemp = malloc(sizeof(item));
    *itemp = item;
    da_append(items, itemp, 1);

    return 0;
}

bool is_in_range(size_t* item, Range* range) {
    return (*item >= range->start && *item <= range->end) ? true : false;
}

int main() {
    char strbuf[1024] = {0};
    bool parse_ranges = true;
    DA* ranges = da_init(100);
    DA* items = da_init(100);
    size_t sum = 0;
    char* filename = "./input.txt";
    FILE* f = fopen(filename, "r");

    while (true) {
        char* line = fgets(strbuf, 1024, f);
        if (line == NULL) break;

        size_t len = strlen(line);
        line[len] = '\0';
        len--;

        if (len == 0) {
            parse_ranges = false;
            continue;
        }

        if (parse_ranges) {
            int r = parse_range(ranges, line, len);
            if (r != 0) {
                printf("Could not parse range: %s\n", line);
                return 1;
            }
        } else {
            int r = parse_item(items, line, len);
            if (r != 0) {
                printf("Could not parse item: %s\n", line);
                return 1;
            }
        }
    }


    // Part 1 solution
    // bool continue_outer = false;
    // for (size_t i = 0; i < items->count; i++) {
    //     size_t* item = da_element(items, i);

    //     for (size_t r = 0; r < ranges->count; r++) {
    //         Range* range = da_element(ranges, r);
    //         if (is_in_range(item, range)) {
    //             sum++;
    //             continue_outer = true;
    //             break;
    //         }
    //     }
    //     if (continue_outer) {
    //         continue_outer = false;
    //         continue;
    //     }
    // }

    // Part 2 solution
    for (size_t i = 0; i < ranges->count; i++) {
        Range* tmp = da_element(ranges, i);
        for (size_t j = tmp->start; j <= tmp->end; j++) {
            sum++;
        }
    }

    printf("%ld\n", sum);
    fclose(f);
    return 0;
}
