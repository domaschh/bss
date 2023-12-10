/**
 * @file generator.c
 * @author Thomas Mages 12124528
 * @brief Generates multiple solutions for the supervisor to consume
 * @details This program generates colored combinations for vertextes in a graph and writes edge removal
 * sets to the shm for the supervisor to consuimes
 * @date 12-10-2023
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared.h"
#include <time.h>

#define MAX_SOL_SIZE 8

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(stderr,"Must give at least 1 edge\n");
        exit(EXIT_FAILURE);
    }
    edge *edges = malloc((argc - 1) * sizeof(edge));

    if (edges == NULL) {
        fprintf(stderr,"Edge alllocatoion failed");
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

    //-----------------Solution Generating---------------
    srand(time(NULL) * getpid());
    int* colors = malloc(sizeof(int) * vertex_ct);
    if (colors == NULL) {
        fprintf(stderr,"color alllocatoion failed");
        free(edges);
        exit(EXIT_FAILURE);
    }
    edge* solution = malloc(sizeof(edge) * MAX_SOL_SIZE);
    if (solution == NULL) {
        fprintf(stderr,"color alllocatoion failed");
        free(edges);
        free(colors);
        exit(EXIT_FAILURE);
    }
    while(1) {
        if(circ_buffer->finished == 1) {
            break;
        }
        //give each node/vertex a random color (represented in colors array)
        for(int i = 0 ; i < vertex_ct;i++) {
            int random_number = rand() % 3;
            colors[i] = random_number;
        }
        
        int curr_sol_count = 0;
        //for each edge check if the nodes it connetcs have the same color 
        //if yes increas solution count and save the edge to the solution for optional debug printing
        for (int i=0;i< edge_ct;i++) {
            if(curr_sol_count == MAX_SOL_SIZE) {
                break;
            }
            if(colors[edges[i].v] == colors[edges[i].u]) {
                solution[curr_sol_count]=edges[i];
                curr_sol_count++;
            }
        }
        //ignore solutions above 8
        if (curr_sol_count == MAX_SOL_SIZE) {
            continue;
        }

        //write solution to the shared memory
        if (curr_sol_count >= 0) {
            if(circ_buffer->finished == 1) {
                break;
            }
            sem_wait(sem_empty);
            if(circ_buffer->finished == 1) {
                break;
            }
            sem_wait(sem_mutex);
            if(circ_buffer->finished == 1) {
                break;
            }
            //----------DEBUG PRINT START-----------
            #ifdef DEBUG
            if (curr_sol_count > 0) {
                printf("DEBUG Solution is: %d\n", curr_sol_count);
                for (int k = 0; k < curr_sol_count; k++) {
                  printf("DEBUG: Edge %d - U: %d, V: %d\n", k, solution[k].u, solution[k].v);
                }
            }
            if (curr_sol_count == 0) {
                printf("----------------\n");
                for(int i = 0 ; i < vertex_ct;i++) {
                    printf("Edge %d has color %d\n", i, colors[i]);
                }
                printf("\nNo edges need to be removed \n");
            }
            #endif
            //----------DEBUG PRINT END-----------

            // fprintf(stderr, "Current writing to %d with val %d\n", circ_buffer->end, curr_sol_count);
            circ_buffer->solutions[circ_buffer->end] = curr_sol_count;
            circ_buffer->end = (circ_buffer->end + 1) % BUFF_SIZE;
            if (circ_buffer->nr_in_use < BUFF_SIZE) {
                circ_buffer->nr_in_use++;
            }
            if(circ_buffer->finished == 1) {
                break;
            }
            // Exit critical section
            sem_post(sem_mutex);
            if(circ_buffer->finished == 1) {
                break;
            }
            // Increment the count of filled slots
            sem_post(sem_filled); 
            if(circ_buffer->finished == 1) {
                break;
            }
        }
    }
    sem_close(sem_filled);
    sem_close(sem_empty);
    sem_close(sem_mutex);
    close(shmfd);
    free(edges);
    free(solution);
    free(colors);
    return 0;
}
