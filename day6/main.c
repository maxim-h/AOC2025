#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "../lib/DynArray.h"

int parse_line(char* line, size_t_da_t* parsed_line) {
    if (*line == '*' || *line == '+') {
        size_t n = 0;
        while (true) {
            if (*(line+n) == '*') {
                size_t_da_append(parsed_line, (size_t)'*', 1);
            } else if (*(line+n) == '+') {
                size_t_da_append(parsed_line, (size_t)'+', 1);
            } else if (*(line+n) == '\n') {
                return 0;
            }
            n++;
        }
    }
    size_t res;
    while (true) {
        if (*line == '\n') break;
        res = strtol(line, &line, 10);
        if (errno == ERANGE) {
            // printf("Overflow or underflow\n");
            return 1;
        } else if (errno == EINVAL && res == 0) {
            continue;
        } else {
            size_t_da_append(parsed_line, res, 1);
        }

    }

    return 0;
}

int solution1(char* filename) {
    size_t sum = 0;
    char strbuf[4096] = {0};
    DA* lines = da_init(2);


    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        // printf("Could not open file: %s\n", filename);
        return 1;
    }

    while (true) {
        char* line = fgets(strbuf, 4096, f);
        if (line == NULL) break;
        size_t_da_t* parsed_line = size_t_da_init(2);
        parse_line(line, parsed_line);
        da_append(lines, parsed_line, 1);
    }

    for (size_t j = 0; j < lines->count-1; j++) {
        size_t_da_t* line = lines->elements[j];
        for (size_t i=0; i < line->count; i++) {
            // printf("%-3ld ", line->elements[i]);
        }
        // printf("\n");
    }
    size_t_da_t* line = lines->elements[lines->count-1];
    for (size_t i=0; i < line->count; i++) {
        // printf("%-3c ", (char)(line->elements[i]));
    }
    // printf("\n");

    for (size_t j=1; j<lines->count; j++) {
    DA* prev_line = lines->elements[j-1];
    DA* line = lines->elements[j];
        if (prev_line->count != line->count) {
            // printf("Lines don't have the same number of elements");
            return 1;
        }
    }

    size_t_da_t* ops = lines->elements[lines->count-1];
    for (size_t i=0; i<ops->count;i++) {
        // printf("Column %ld\n", i);
        size_t col_res=0;
        if ((char)ops->elements[i] == '+') {
            col_res = 0;
            for (size_t j=0; j < lines->count-1; j++) {
                size_t_da_t* line = lines->elements[j];
                // printf("Adding %ld to %ld ;", line->elements[i], col_res);
                col_res += line->elements[i];
            }
        // printf("\n");
        } else if ((char)ops->elements[i] == '*') {
            col_res = 1;
            for (size_t j=0; j < lines->count-1; j++) {
                size_t_da_t* line = lines->elements[j];
                // printf("Multiplying %ld by %ld ;", col_res, line->elements[i]);
                col_res *= line->elements[i];
            }
        // printf("\n");
        } else {
            // printf("Oops");
            return 1;
        }
        // // printf("Adding %ld\n", col_res);
        sum += col_res;
    }


    printf("%ld\n", sum);

    fclose(f);
    return 0;
}

int parse_ops(char* line, size_t_da_t* ops, size_t_da_t* lengths) {
        size_t n = 0;
        size_t n_prev = 0;
        while (true) {
            if (*(line+n) == '*') {
                // printf("Found operation %c\n", '*');
                size_t_da_append(ops, (size_t)'*', 1);
                if (n_prev != n) {
                    // printf("Previous op length: %ld\n", n - n_prev);
                    size_t_da_append(lengths, n - n_prev - 1, 1);
                }
                n_prev = n;
            } else if (*(line+n) == '+') {
                // printf("Found operation %c\n", '+');
                size_t_da_append(ops, (size_t)'+', 1);
                if (n_prev != n) {
                    // printf("Previous op length: %ld\n", n - n_prev);
                    size_t_da_append(lengths, n - n_prev - 1, 1);
                }
                n_prev = n;
            } else if (*(line+n) == '\n') {
                    // printf("Found newline\n");
                    // printf("Previous op length: %ld\n", n - n_prev);
                    size_t_da_append(lengths, n - n_prev, 1);
                return 0;
            }
            n++;
        }

    return 0;
}

int init_results(DA* results, size_t_da_t* ops, size_t_da_t* lengths) {
    for (size_t i=0; i < ops->count; i++) {
        DA* res = da_init(1);
        size_t len = lengths->elements[i];
        for (size_t j=0; j < len; j++) {
            size_t_da_t* r = size_t_da_init(1);
            da_append(res, r, 1);
        }
        da_append(results, res, 1);
        // if ((char)ops->elements[i] == '*') {
        //     size_t_da_append(results, 1, 1);
        //     // results->elements[i] = 1;
        // } else if ((char)ops->elements[i] == '+') {
        //     size_t_da_append(results, 0, 1);
        //     // results->elements[i] = 0;
        // } else {
        //     printf("Unexpected operation encountered: %c\n", (char)ops->elements[i]);
        //     return 1;
        // }
    }
    return 0;
}

int aggregate_line(char* line, size_t multiplier, size_t_da_t* ops, size_t_da_t* lengths, DA* results) {
        if (*line == '*' || *line == '+') return 0;

        // printf("%s", line);
        size_t sp = 0;
        for (size_t i = 0; i < ops->count; i++) {
            char op = ops->elements[i];
            size_t l = lengths->elements[i];
            DA* res = results->elements[i];
            // printf("Parsing op %c with length %ld\n", op, l);
            for (size_t char_index = 0; char_index < l; char_index++) {
                char c = *(line + sp + char_index);

                if (c == ' ') continue;

                size_t_da_t* r = res->elements[char_index];
                size_t_da_append(r, (size_t)(c - '0'), 1);
            }
            // printf("\nJumping to position: %ld.\n", sp+l+1);
            sp = sp + l + 1;
        }

    return 0;
}

int solution2(char* filename) {
    size_t sum = 0;
    char strbuf[4096] = {0};
    size_t nlines = 0;
    DA* lines = da_init(2);
    size_t_da_t* lengths = size_t_da_init(2);
    size_t_da_t* ops = size_t_da_init(2);
    DA* results = da_init(2);


    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        // printf("Could not open file: %s\n", filename);
        return 1;
    }

    // get to the last line and parse the ops first
    while (true) {
        char* line = fgets(strbuf, 4096, f);
        if (line == NULL) break;
        nlines++;
    }

    if (parse_ops(strbuf, ops, lengths) != 0) return 1;

    // print ops and lengths
    // for (size_t i = 0; i < ops->count-1; i++) {
    //     printf("%c: %ld; ", (char)ops->elements[i], lengths->elements[i]);
    // }
    // printf("\n");

    fclose(f);

    if (init_results(results, ops, lengths) != 0) return 1;

    f = fopen(filename, "r");

    size_t line_number = 0;
    while (true) {
        char* line = fgets(strbuf, 4096, f);
        if (line == NULL) break;
        // printf("%s", line);
        line_number++;
        size_t multiplier = pow(10, nlines - line_number);
        if (aggregate_line(line, multiplier, ops, lengths, results) != 0) {
            printf("Could not aggregate line informatoion.");
            return 1;
        }
    }

    if (results->count != ops->count) {
        printf("Results array has more elements (%ld) than columns in input data: %ld\n", results->count, ops->count);
        return 1;
    }

    for (size_t i=0; i < results->count; i++) {
        DA* res = results->elements[i];
        char op = ops->elements[i];
        size_t l = res->count;
        size_t local_res = 0;
        if (op == '+') {
            local_res = 0;
            for (size_t j=0; j < l; j++) {
                size_t number = 0;
                size_t_da_t* r = res->elements[j];
                for (size_t k=0; k<r->count; k++) {
                    number += r->elements[k] * pow(10, r->count - k - 1);
                }
                // printf("%ld + ", number);
                local_res += number;
            }
        } else if (op == '*') {
            local_res = 1;
            for (size_t j=0; j < l; j++) {
                size_t number = 0;
                size_t_da_t* r = res->elements[j];
                for (size_t k=0; k<r->count; k++) {
                    number += r->elements[k] * pow(10, r->count - k - 1);
                }
                // printf("%ld + ", number);
                local_res *= number;
            }
        } else return 1;
        // printf("= %ld\n", local_res);
        sum += local_res;
        // printf("%ld ", results->elements[i]);
    }
    printf("\n");

    printf("%ld\n", sum);

    fclose(f);
    return 0;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Exactly one command-line argument expected - input filename.\n");
        printf("%d provided.\n", argc - 1);
        return 1;
    }

    char* filename = argv[1];

    // return solution1(filename);
    return solution2(filename);
}
