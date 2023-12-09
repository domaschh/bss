#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>           // Include for O_CREAT and other flags
#include <sys/mman.h>
#include <sys/types.h>
#include <semaphore.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#define BUFF_SIZE 10
/**
 * struct that holds an edge
 * u = from
 * v = to
 */
typedef struct edge {
    int u;
    int v;
} edge;

/**
 * A Struct holding an array f edges that represent a solution-set
 * sets with more than 8 edges can be discarded according to the Angabe
 */
typedef struct solution {
    edge remove_edges[8];
    int nr_edges;
} solution;

#define SHM_ID "/12124528_SHM"
/**
 * Circular buffeer of size 10 that stores an array of solutions
 * nr_in_use = how many solutions are currently in the buffer
 */
typedef struct circular_buffer {
    int solutions[BUFF_SIZE];
    int start;
    int end;
    int nr_in_use;
    int finished;
} circular_buffer;

#define SEM_FILLED_ID "/12124528_SEM_FILLED"
#define SEM_EMPTY_ID "/12124528_SEM_EMPTY"
#define SEM_MUTEX_NAME "/12124528_SEM_MUTEX"

sem_t *sem_filled;
sem_t *sem_empty;
sem_t *sem_mutex;