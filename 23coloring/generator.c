#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared.h"

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        perror("Must give at least 1 edge");
        exit(EXIT_FAILURE);
    }
    edge *edges = malloc((argc - 1) * sizeof(edge));

    if (edges == NULL) {
        perror("Edge alllocatoion failed");
        exit(EXIT_FAILURE);
    }

    int vertex_ct = 0;
    for (int i = 1; i < argc; i++) {
        char *token = strtok(argv[i], "-");
        if (token == NULL) {
            fprintf(stderr, "Invalid format: %s\n", argv[i]);
            free(edges);
            exit(EXIT_FAILURE);
        }
        
        char *endptr;
        edges[i-1].u = (int) strtol(token, &endptr, 10); // Convert first part to integer
        if (*endptr != '\0') {
            fprintf(stderr, "Invalid number: %s\n", token);
            free(edges);
            exit(EXIT_FAILURE);
        }

        token = strtok(NULL, "-");
        if (token == NULL) {
            fprintf(stderr, "Invalid format: %s\n", argv[i]);
            free(edges);
            exit(EXIT_FAILURE);
        }

        edges[i-1].v = (int) strtol(token, &endptr, 10); // Convert second part to integer
        if (*endptr != '\0') {
            fprintf(stderr, "Invalid number: %s\n", token);
            free(edges);
            exit(EXIT_FAILURE);
        }
        vertex_ct = edges[i-1].u > vertex_ct ? edges[i-1].u : vertex_ct;
        vertex_ct = edges[i-1].v > vertex_ct ? edges[i-1].v : vertex_ct;
        printf("Parsed edge: %d-%d\n", edges[i-1].u, edges[i-1].v);
    }
    vertex_ct++;
    printf("\n Vertex Count: %d\n", vertex_ct);
    int* colors = malloc(sizeof(int) * vertex_ct);
     if (colors == NULL) {
        perror("color alllocatoion failed");
        free(edges);
        exit(EXIT_FAILURE);
    }
    int edge_ct = argc-1;

    while(1) {
        srand(time(NULL));
        edge* solution = malloc(sizeof(edge) * 8);
        
        for(int i = 0 ; i < vertex_ct;i++) {
            int random_number = rand() % 3;
            colors[i] = random_number;
            printf("Color for Vertex: %d = %d\n", i, colors[i]);
        }
        

        int j = 0;
        for (int i=0;i< edge_ct;i++) {
            if(j == 8) {
                printf("Solution too big \n");
                free(solution);
                free(edges);
                free(colors);
                break;
            }
            if(colors[edges[i].v] == colors[edges[i].u]) {
                printf("Same Colored edge: U = %d, V = %d\n C= %d\n", edges[i].v, edges[i].v, colors[edges[i].v]);
                solution[j]=edges[i];
                j++;
            }
        }
        if (j == 8) {
            free(solution);
            continue;
        }

        if (j==0) {
            free(solution);
            printf("Graph is 3 colorable\n");
            break;
        }
        for (int i = 0; i < j; i++) {
            printf("Solution found with\n");
            printf("(U = %d, V = %d)", solution[i].u, solution[i].v);
        }
        free(solution);
    }
    free(colors);
    free(edges);
    return 0;
}
