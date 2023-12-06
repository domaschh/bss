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

/**
 * Circular buffeer of size 10 that stores an array of solutions
 * nr_in_use = how many solutions are currently in the buffer
 */
typedef struct circular_buffer {
    solution solutions[10];
    int start;
    int end;
    int nr_in_use;
    sem_t *sem_filled;
    sem_t *sem_empty;
    sem_t *sem_mutex;
} circular_buffer;
