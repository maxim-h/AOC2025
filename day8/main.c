#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../lib/DynArray.h"

typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
} Point;

typedef struct {
    Point *p1;
    Point *p2;
    double d;
} PDist;

Point parse_line(char* line) {
    Point p = {0};

    size_t ll = strlen(line);
    line[ll-1] = '\0';
    // printf("%s\n", line);

    char *prev = strsep(&line, ",");
    p.x = strtol(prev, NULL, 10);
    prev = strsep(&line, ",");
    p.y = strtol(prev, NULL, 10);
    p.z = strtol(line, NULL, 10);
    return p;
}

static inline size_t dist_index(size_t i, size_t j, size_t n)
{
    /* assumes i < j */
    return i * n - (i * (i + 1)) / 2 + (j - i - 1);
}


PDist dist(Point *p1, Point *p2) {
    double d = sqrt(pow(p1->x - p2->x, 2) + pow(p1->y - p2->y, 2) + pow(p1->z - p2->z, 2));
    PDist r = {p1, p2, d};
    return r;
}

int dist_comp(const void *pdist1, const void *pdist2) {
    const PDist *d1 = pdist1;
    const PDist *d2 = pdist2;

    if (d1->d > d2->d) {
        return 1;
    } else if (d1->d < d2->d) {
        return -1;
    } else {
        return 0;
    }

}

int component_comp(const void *pcomp1, const void *pcomp2) {
    const void *const *p1 = pcomp1;
    const void *const *p2 = pcomp2;
    const Vec *c1 = *p1;
    const Vec *c2 = *p2;

    if (c1->count < c2->count) {
        return 1;
    } else if (c1->count > c2->count) {
        return -1;
    } else {
        return 0;
    }

}

void print_component(Vec *el) {
    printf("%ld component: ", el->count);
    for (size_t j=0; j<el->count; j++) {
        void **pp = vec_element(el, j);
        Point *p = *pp;
        if (j > 0) printf("<->");
        printf("{%d,%d,%d}", p->x, p->y, p->z);
    }
    printf("\n");
}

int solution1(char *filename) {
    char strbuf[4096] = {0};
    FILE *f = fopen(filename, "r");
    // Vec *components = vec_init(1, sizeof(Vec));
    Vec *points = vec_init(1, sizeof(Point));
    size_t n_integrations = 1000;
    size_t top_n = 3;
    size_t result = 1;

    while(true) {
        char* line = fgets(strbuf, sizeof(strbuf), f);
        if (line == NULL) break;

        Point p = parse_line(line);
        vec_append(points, &p, 1);
        // Vec *comp = vec_init(1, sizeof(Point));
        // vec_append(comp, &p, 1);
        // vec_append(components, comp, 1);
    }

    size_t N = points->count;
    size_t N_dists = N*(N-1)/2;
    PDist *dists = calloc(N_dists, sizeof *dists);

    for (size_t i=0; i < N; i++) {
        for (size_t j=i+1; j < N; j++) {
            dists[dist_index(i, j, N)] = dist(vec_element(points, i), vec_element(points, j));
            PDist pd = dists[dist_index(i, j, N)];
            // printf("{%d, %d, %d} - {%d, %d, %d}: %f\n", pd.p1->x, pd.p1->y, pd.p1->z, pd.p2->x, pd.p2->y, pd.p2->z, pd.d);
        }
    }

    qsort(dists, N_dists, sizeof(PDist), &dist_comp);

    // Fill componets with 2*n_integrations disconnected elements
    // That should be the maximum number of elements comprising
    // Not quite sure about that
    Vec *components = vec_init(n_integrations, sizeof(void *));
    size_t n_integrations_done = 0;
    for (size_t i = 0; i < N_dists; i++) {
        if (n_integrations_done >= n_integrations) break; // stop after n_integrations
        PDist pd = dists[i];
        // printf("Adding PDist {%d,%d,%d}<->{%d,%d,%d}\n", pd.p1->x, pd.p1->y, pd.p1->z, pd.p2->x, pd.p2->y, pd.p2->z);
        // bool in_components = false;

        // printf("Currently %ld components present:\n", components->count);
        Vec *p1_in = NULL;
        Vec *p2_in = NULL;
        for (size_t j=0; j<components->count; j++) {
            void **pcomponent = vec_element(components, j);
            Vec *component = *pcomponent;
            // print_component(component);


            // iterate over points in a component
            // bool p1_in = false;
            // bool p2_in = false;
            for (size_t k=0; k < component->count; k++) {
                void **pp = vec_element(component, k);
                Point *p = *pp;
                // printf("Comparing existing point {%d,%d,%d} to new points: {%d,%d,%d} and {%d,%d,%d}\n", p->x, p->y, p->z, pd.p1->x, pd.p1->y, pd.p1->z, pd.p2->x, pd.p2->y, pd.p2->z);
                if (p == pd.p1) {
                    p1_in = component;

                    // in_components = true;
                    // printf("Existing component has point {%d,%d,%d}, ", p->x, p->y, p->z);
                }
                if (p == pd.p2) {
                    p2_in = component;
                    // in_components = true;
                    // printf("Existing component has point {%d,%d,%d}, ", p->x, p->y, p->z);
                }
            }

            // if (p1_in && p2_in) {
            //     continue;
            // } else if (p1_in) {
            //         vec_append(component, &(pd.p2), 1);
            //         // printf("Appending new point {%d,%d,%d}\n", pd.p2->x, pd.p2->y, pd.p2->z);
            // } else if (p2_in) {
            //         vec_append(component, &(pd.p1), 1);
            //         // printf("Appending new point {%d,%d,%d}\n", pd.p1->x, pd.p1->y, pd.p1->z);
            // }
        }

        if (p1_in == NULL && p2_in == NULL) {
            // completely new component
            Vec *component = vec_init(2, sizeof(void *));
            vec_append(component, &(pd.p1), 1);
            vec_append(component, &(pd.p2), 1);
            // printf("Adding new component: {%d,%d,%d}<->{%d,%d,%d}\n", pd.p1->x, pd.p1->y, pd.p1->z, pd.p2->x, pd.p2->y, pd.p2->z);
            vec_append(components, &component, 1);
            n_integrations_done++; // advance counter
        } else if (p1_in == p2_in) {
            // Both points are in the same existing component - skip iteration
            // printf("Both points already present\n");
            n_integrations_done++; // advance counter
            continue;
        } else if (p1_in == NULL) {
            // p2 is present in one of the components
            vec_append(p2_in, &(pd.p1), 1);
            n_integrations_done++; // advance counter
        } else if (p2_in == NULL) {
            // p1 is present in one of the components
            vec_append(p1_in, &(pd.p2), 1);
            n_integrations_done++; // advance counter
        } else {
            // Both points are present, but in different components
            // Need to merge the components. Will merge p2_in into p1_in and
            // swap p2_in with an empty component
            // printf("Merging 2 components together.\n");
            for (size_t j=0; j < p2_in->count; j++) {
                void **pp = vec_element(p2_in, j);
                // Point *p = *pp;
                vec_append(p1_in, pp, 1);
            }
            free(p2_in->elements);
            p2_in->count = 0;
            n_integrations_done++; // advance counter
        }
    }

    qsort(components->elements, components->count, components->element_size, &component_comp);


    // printf("Final components:\n");
    for (size_t i = 0; i < top_n; i++){
        void **pel = vec_element(components, i);
        if (pel == NULL) {
            printf("Failed to retrieve element %ld from components array.\n", i);
            return 1;
        }
        Vec *el = *pel;

        result *= el->count;
        // print_component(el);
    }

    printf("%ld\n", result);

    return 0;
}

size_t n_components(const Vec *components) {
    size_t n = 0;

    for (size_t i = 0; i < components->count; i++) {
        const void *const *pcomponent = vec_element(components, i);
        const Vec *component = *pcomponent;
        if (component->count > 0) n++;
    }

    return n;
}

int solution2(char *filename) {
    char strbuf[4096] = {0};
    FILE *f = fopen(filename, "r");
    // Vec *components = vec_init(1, sizeof(Vec));
    Vec *points = vec_init(1, sizeof(Point));
    size_t n_integrations = 1000;
    size_t top_n = 3;
    size_t result = 1;

    while(true) {
        char* line = fgets(strbuf, sizeof(strbuf), f);
        if (line == NULL) break;

        Point p = parse_line(line);
        vec_append(points, &p, 1);
        // Vec *comp = vec_init(1, sizeof(Point));
        // vec_append(comp, &p, 1);
        // vec_append(components, comp, 1);
    }

    size_t N = points->count;
    size_t N_dists = N*(N-1)/2;
    PDist *dists = calloc(N_dists, sizeof *dists);

    for (size_t i=0; i < N; i++) {
        for (size_t j=i+1; j < N; j++) {
            dists[dist_index(i, j, N)] = dist(vec_element(points, i), vec_element(points, j));
            PDist pd = dists[dist_index(i, j, N)];
            // printf("{%d, %d, %d} - {%d, %d, %d}: %f\n", pd.p1->x, pd.p1->y, pd.p1->z, pd.p2->x, pd.p2->y, pd.p2->z, pd.d);
        }
    }

    qsort(dists, N_dists, sizeof(PDist), &dist_comp);

    // Fill componets with 2*n_integrations disconnected elements
    // That should be the maximum number of elements comprising
    // Not quite sure about that
    Vec *components = vec_init(N, sizeof(Vec *));

    // First add all points as separate components
    for (size_t i=0; i < N; i++) {
            Vec *component = vec_init(1, sizeof(Point *));
            Point *p = vec_element(points, i);
            vec_append(component, &p, 1);
            vec_append(components, &component, 1);
    }

    // aggregation loop
    for (size_t i = 0; i < N_dists; i++) {
        PDist pd = dists[i];

        Vec *p1_in = NULL;
        Vec *p2_in = NULL;
        for (size_t j=0; j<components->count; j++) {
            void **pcomponent = vec_element(components, j);
            Vec *component = *pcomponent;
            // print_component(component);


            // iterate over points in a component
            // bool p1_in = false;
            // bool p2_in = false;
            for (size_t k=0; k < component->count; k++) {
                void **pp = vec_element(component, k);
                Point *p = *pp;
                // printf("Comparing existing point {%d,%d,%d} to new points: {%d,%d,%d} and {%d,%d,%d}\n", p->x, p->y, p->z, pd.p1->x, pd.p1->y, pd.p1->z, pd.p2->x, pd.p2->y, pd.p2->z);
                if (p == pd.p1) {
                    p1_in = component;

                    // in_components = true;
                    // printf("Existing component has point {%d,%d,%d}, ", p->x, p->y, p->z);
                }
                if (p == pd.p2) {
                    p2_in = component;
                    // in_components = true;
                    // printf("Existing component has point {%d,%d,%d}, ", p->x, p->y, p->z);
                }
            }

            // if (p1_in && p2_in) {
            //     continue;
            // } else if (p1_in) {
            //         vec_append(component, &(pd.p2), 1);
            //         // printf("Appending new point {%d,%d,%d}\n", pd.p2->x, pd.p2->y, pd.p2->z);
            // } else if (p2_in) {
            //         vec_append(component, &(pd.p1), 1);
            //         // printf("Appending new point {%d,%d,%d}\n", pd.p1->x, pd.p1->y, pd.p1->z);
            // }
        }

        if (p1_in == NULL && p2_in == NULL) {
            // completely new component
            Vec *component = vec_init(2, sizeof(void *));
            vec_append(component, &(pd.p1), 1);
            vec_append(component, &(pd.p2), 1);
            // printf("Adding new component: {%d,%d,%d}<->{%d,%d,%d}\n", pd.p1->x, pd.p1->y, pd.p1->z, pd.p2->x, pd.p2->y, pd.p2->z);
            vec_append(components, &component, 1);
        } else if (p1_in == p2_in) {
            // Both points are in the same existing component - skip iteration
            // printf("Both points already present\n");
            continue;
        } else if (p1_in == NULL) {
            // p2 is present in one of the components
            vec_append(p2_in, &(pd.p1), 1);
        } else if (p2_in == NULL) {
            // p1 is present in one of the components
            vec_append(p1_in, &(pd.p2), 1);
        } else {
            // Both points are present, but in different components
            // Need to merge the components. Will merge p2_in into p1_in and
            // swap p2_in with an empty component
            // printf("Merging 2 components together.\n");
            for (size_t j=0; j < p2_in->count; j++) {
                void **pp = vec_element(p2_in, j);
                // Point *p = *pp;
                vec_append(p1_in, pp, 1);
            }
            free(p2_in->elements);
            p2_in->count = 0;
        }

        if (n_components(components) == 1) {
            result = pd.p1->x * pd.p2->x;
        };
    }


    printf("%ld\n", result);

    return 0;
}
int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Exactly one command-line argument expected - input filename.\n");
        printf("%d provided.\n", argc - 1);
    }
    char *filename = argv[1];
    return solution2(filename);
}
