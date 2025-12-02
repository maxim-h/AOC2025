#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// part 1
void check_number1(unsigned long i, unsigned long* sum) {
    char istr[1024] = {0};

    snprintf(istr, sizeof(istr), "%ld", i);

    size_t l = strnlen(istr, 1024);

    div_t div_res = div(l, 2);
    if (div_res.rem != 0) return;

    // printf("Number: %d. String: %s. Half string right: %s.\n", i, istr, istr+div_res.quot);
    // printf("Comparison result: %d\n", strncmp(istr, istr+div_res.quot, div_res.quot));

    if (strncmp(istr, istr+div_res.quot, div_res.quot) == 0) {
        // printf("GOTIM\n");
        *sum += i;
    }
    return;
}

// part 2
void check_number2(unsigned long num, unsigned long* sum) {
    char istr[1024] = {0};

    snprintf(istr, sizeof(istr), "%ld", num);

    size_t l = strnlen(istr, 1024);

    div_t div_res = div(l, 2);

    // printf("%s\n", istr);

    // single digit number
    if ((div_res.quot == 0) && (div_res.rem == 1)) return;

    // iterate over prefix sizes until lower(half-length) of string
    bool should_continue = false;
    for (unsigned int i=1; i <= div_res.quot; i++) {
        div_t chunk_div = div(l, i);
        // printf("-----------------------------------\n");
        // printf("i: %d, Quotient: %d, Remainder: %d\n", i, chunk_div.quot, chunk_div.rem);
        if (chunk_div.rem != 0) continue;


        // iterate over chunks starting from the second one
        for (unsigned int j = 1; j < chunk_div.quot; j++) {
            // printf("j: %d\n", j);
            // printf("Compatison: %d\n", strncmp(istr + (j-1)*i , istr + j*i, i));
            if (strncmp(istr + (j-1)*i , istr + j*i, i) != 0) {
                should_continue = true;
                break;
            }
        }

        if (should_continue) {
            should_continue = false;
            continue;
        } else {
            // printf("Found one: %ld\n", num);
            *sum += num;
            break;
        }

    }

    // printf("Final sum: %ld", *sum);

    // printf("Number: %d. String: %s. Half string right: %s.\n", i, istr, istr+div_res.quot);
    // printf("Comparison result: %d\n", strncmp(istr, istr+div_res.quot, div_res.quot));

    return;
}


int main() {
    char* filename = "./input.txt";
    FILE* f = fopen(filename, "r");
    char* line = NULL;
    size_t linecap = 0;
    char num_separator = '-';
    unsigned long sum = 0;

    while (true) {
        int nchar = getdelim(&line, &linecap, (int)',',f);
        if (nchar < 0) break;
        line[nchar-1] = '\0';

        char* start_str = strsep(&line, &num_separator);
        if (start_str == NULL) return 1;
        if (*start_str == '\0') return 1; // I don't know if that actually works ;)
        unsigned long range_start = strtol(start_str, NULL, 10);
        unsigned long range_end = strtol(line, NULL, 10);

        for (unsigned long i = range_start; i <= range_end; i++) check_number2(i, &sum);

        line = NULL; // reset the line to continue iteration
    }

    printf("%ld\n", sum);

    fclose(f);
    return 0;
}
