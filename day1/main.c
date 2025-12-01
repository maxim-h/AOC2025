#include <_stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

int parse_line(char* line) {
    int rotation = strtol(line+1, NULL, 10);
    if (*line == 'R') {
        return rotation;
    } else if (*line == 'L') {
        return -1 * rotation;
    } else {
        printf("Error parsing line:\n");
        printf("%s", line);
        return 1;
    }
}

int rotate(int cur, int rotation, int denom, int* n_zero) {
    div_t res = div(cur + rotation, denom);
    int new_pos = res.rem;


    if (new_pos < 0) {
        if (cur > 0) {
            *n_zero -= res.quot - 1;
        } else {
            *n_zero -= res.quot;
        }
        return denom + new_pos;
    } else if (new_pos == 0) {
        if (res.quot > 0) {
            *n_zero += res.quot;
        } else if (res.quot < 0) {
            *n_zero -= res.quot - 1;
        } else {
            *n_zero += 1;
        }
        return new_pos;
    } else {
        *n_zero += res.quot;
        return new_pos;
    }
}

int solution1() {
    int dial = 50;
    int n_zero = 0;
    char* filename = "./input.txt";
    char strbuf[1024] = {0};
    FILE * f = fopen(filename, "r");

    while (true) {
        char* line = fgets(strbuf, 1024, f);
        if (line == NULL) break;
        int rotation = parse_line(line);
        printf("Current dial position: %d\n", dial);
        printf("Operation: %s", line);
        dial = rotate(dial, rotation, 100, &n_zero);
        printf("New dial position: %d\n", dial);
        if (dial == 0) {
            n_zero += 1;
        }
    }

    printf("Finished successfully\n");
    printf("%d\n", n_zero);

    fclose(f);
    return(0);
}

int solution2() {
    int dial = 50;
    int n_zero = 0;
    int n_zero_prev = 0;
    char* filename = "./input1.txt";
    char strbuf[1024] = {0};
    FILE * f = fopen(filename, "r");

    while (true) {
        n_zero_prev = n_zero;
        char* line = fgets(strbuf, 1024, f);
        if (line == NULL) break;
        int rotation = parse_line(line);
        // printf("Current dial position: %d. Current n: %d\n", dial, n_zero);
        // printf("Operation: %s", line);
        dial = rotate(dial, rotation, 100, &n_zero);

    }

    printf("Finished successfully\n");
    printf("%d\n", n_zero);

    fclose(f);
    return 0;
}

int main() {
    // return solution1();
    return solution2();

}
