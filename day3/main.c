#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

size_t largest_n(uint32_t* digits, uint32_t l, uint32_t n) {
    uint32_t first_largest = 0;
    uint32_t second_largest = 0;
    uint32_t* result = malloc(n * sizeof(uint32_t));
    uint64_t sum = 0;

    size_t max_pos = 0;
    for (uint32_t digit = 0; digit < n; digit++){
        uint32_t cur_largerst = 0;

        // how many digits at the end still need to be available
        uint32_t slack = n - digit - 1;

        for (size_t i=max_pos; i < l - slack; i++) {
            if (digits[i] > result[digit]) {
                result[digit] = digits[i];
                max_pos = i+1;
            }
        }
    }

    for (size_t i=0; i<n; i++) {
        sum += pow(10, n - i - 1) * result[i];
    }

    return sum;
}

int main() {
    long sum = 0;
    char* filename = "./input.txt";
    char strbuf[1024] = {0};
    FILE* f = fopen(filename, "r");

    while (true) {
        char* line = fgets(strbuf, 1024, f);
        if (line == NULL) break;

        size_t l = strlen(line);
        l--;

        line[l] = '\0';

        uint32_t* digits = malloc(l * sizeof(uint32_t));

        for (size_t i=0; i<l; i++) {
        digits[i] = line[i] - '0';
        }

        sum += largest_n(digits, l, 12);
    }

    printf("%ld\n", sum);

    return 0;
}
