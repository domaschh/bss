#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared.h"

#define MAX_SOL_SIZE 8

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(stderr,"Must give at least 1 edge\n");
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
    }
    vertex_ct++;
    int* colors = malloc(sizeof(int) * vertex_ct);
     if (colors == NULL) {
        perror("color alllocatoion failed");
        free(edges);
        exit(EXIT_FAILURE);
    }
    int edge_ct = argc-1;

    //-----------------------SHM---------------------
    sem_filled = sem_open(SEM_FILLED_ID, 0);
    if (sem_filled == SEM_FAILED) {
        fprintf(stderr, "opening sem_filled failed\n");
        exit(EXIT_FAILURE);
    }

    // occupied space semaphore
    sem_empty = sem_open(SEM_EMPTY_ID, 0);
    if (sem_empty == SEM_FAILED) {
        fprintf(stderr, "opening sem_empty failed\n");
        exit(EXIT_FAILURE);
    }

    // write-access semaphore
    sem_mutex = sem_open(SEM_MUTEX_NAME, 0);
    if (sem_mutex == SEM_FAILED) {
        fprintf(stderr, "opening sem_mutex failed\n");
        exit(EXIT_FAILURE);
    }

    int shmfd = shm_open(SHM_ID, O_RDWR, 0);
    if (shmfd == -1) {
        fprintf(stderr, "opening shm failed failed");
        exit(EXIT_FAILURE);
    }
    circular_buffer* circ_buffer = mmap(NULL, sizeof(struct circular_buffer), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if (circ_buffer == MAP_FAILED) {
        fprintf(stderr, "mmapping circ buff failed");
        exit(EXIT_FAILURE);
    }
    //---------------------SHMEND-----------------

    while(1) {
        srand(time(NULL));
        edge* solution = malloc(sizeof(edge) * MAX_SOL_SIZE);
        
        for(int i = 0 ; i < vertex_ct;i++) {
            int random_number = rand() % 3;
            colors[i] = random_number;
        }
        

        int j = 0;
        for (int i=0;i< edge_ct;i++) {
            if(j == MAX_SOL_SIZE) {
                free(solution);
                free(edges);
                free(colors);
                break;
            }
            if(colors[edges[i].v] == colors[edges[i].u]) {
                solution[j]=edges[i];
                j++;
            }
        }
        if (j == MAX_SOL_SIZE) {
            free(solution);
            continue;
        }

        if (j >= 0) {
            sem_wait(sem_empty); // Wait for an empty slot
            sem_wait(sem_mutex); // Enter critical section
            #ifdef DEBUG
            if (j > 0) {
                printf("DEBUG Solution is: %d\n", j);
                for (int k = 0; k < j; k++) {
                  printf("DEBUG: Edge %d - U: %d, V: %d\n", k, solution[k].u, solution[k].v);
                }
            }
            if (j == 0) {
                printf("No edges need to be removed");
            }
            #endif
            circ_buffer->solutions[circ_buffer->end] = j;
            circ_buffer->end = (circ_buffer->end + 1) % BUFF_SIZE;
            if (circ_buffer->nr_in_use < BUFF_SIZE) {
                circ_buffer->nr_in_use++;
            }

            sem_post(sem_mutex); // Exit critical section
            sem_post(sem_filled); // Increment the count of filled slots
        }
        free(solution);
    }
    free(colors);
    free(edges);
    return 0;
}
