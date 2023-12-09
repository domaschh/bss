#include "shared.h"
#include <unistd.h> 

volatile sig_atomic_t terminate = 0;

void handle_signal(int sig) {
    terminate = 1;
}

int main(int argc, char *argv[]) {
    int limit = INT_MAX;
    int delay = 0;
    char* check;
    //needed for it to always work
    sem_unlink(SEM_FILLED_ID);
    sem_unlink(SEM_EMPTY_ID);
    sem_unlink(SEM_MUTEX_NAME);
    int opt;

    while ((opt = getopt(argc, argv, "n:w:")) != -1) {
        switch (opt) {
            case 'n':
                limit = strtoul(optarg, &check, 10);
                if (*check != '\0') {
                    fprintf(stderr, "Invalid number for -n: %s\n", optarg);
                    return EXIT_FAILURE;
                }
                break;
            case 'w':
                delay = strtoul(optarg, &check, 10);
                if (*check != '\0') {
                    fprintf(stderr, "Invalid number for -n: %s\n", optarg);
                    return EXIT_FAILURE;
                }
                break;
            default: /* '?' */
                fprintf(stderr, "Usage: %s [-n limit] [-w delay]\n", argv[0]);
                return EXIT_FAILURE;
        }
    }

    #ifdef DEBUG
    printf("\n Limit for solutions to read: %d", limit);
    printf("\n Delay: %d \n", delay);
    #endif

    //-------------------------------SHM & SEM Mapping----------------------
    int opened = shm_open(SHM_ID, O_RDWR | O_CREAT, 0600);

    if (opened == -1) {
        fprintf(stderr, "Failed call to shm_open: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    if(ftruncate(opened, sizeof(struct circular_buffer)) == -1 ) {
        close(opened);
        fprintf(stderr, "Failed to truncate shared memory");
        return EXIT_FAILURE;
    }

    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED;
    circular_buffer *circ_buffer = mmap(0, sizeof(circular_buffer), protection, visibility,opened,0);

    if (circ_buffer == MAP_FAILED) {
        close(opened);
        fprintf(stderr, "Failed to call mmap()");
        return EXIT_FAILURE;
    }

    circ_buffer->start = 0;
    circ_buffer->end = 0;
    circ_buffer->nr_in_use = 0;
    circ_buffer->finished = 0;

    sleep(delay);

    sem_filled = sem_open(SEM_FILLED_ID, O_CREAT | O_EXCL, 0600, 0);
    if (sem_filled == SEM_FAILED) {
        perror("sem_open sem_filled");
        exit(EXIT_FAILURE);
    }

    sem_empty = sem_open(SEM_EMPTY_ID, O_CREAT | O_EXCL, 0600, 10); // Assuming buffer size is 10
    if (sem_empty == SEM_FAILED) {
        perror("sem_open sem_empty");
        exit(EXIT_FAILURE);
    }

    sem_mutex = sem_open(SEM_MUTEX_NAME, O_CREAT | O_EXCL , 0600, 1);
    if (sem_mutex == SEM_FAILED) {
        perror("sem_open sem_mutex");
        exit(EXIT_FAILURE);
    }

    int best_solution = INT_MAX;

    // Setting up signal handling
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    // Wait for delay seconds
    sleep(delay);

    int solutions_read = 0;
    //-------------------------------Solution reading----------------------

    while (terminate == 0) {
        if (solutions_read == limit) {
            terminate = 1;
            break; 
        }
        if (sem_trywait(sem_filled) == 0) {
            // Read a solution from the buffer
            // fprintf(stderr,"Reading from %d\n", circ_buffer->start);

            // int value;
            // sem_getvalue(sem_empty, &value);

            // fprintf(stderr," sem_empty %d\n", value);
            // for (int x = 0; x<BUFF_SIZE; x++) {
            //     fprintf(stderr, "Value in Buff is: %d\n", circ_buffer->solutions[x]);
            // }
            int current_solution = circ_buffer->solutions[circ_buffer->start];
            circ_buffer->start = (circ_buffer->start + 1) % BUFF_SIZE; // Circular increment
            circ_buffer->nr_in_use--;
            solutions_read++;

            // Unlock the buffer
            sem_post(sem_mutex);

            // Signal that a slot is now empty
            sem_post(sem_empty);

            // Process the solution
            if (current_solution == 0) {
                printf("The graph is 3-colorable!\n");
                circ_buffer->finished = 1;
                break;
            }

            if (current_solution < best_solution) {
                best_solution = current_solution;
                fprintf(stderr, "New best solution with %d edges.\n", best_solution);
            }
        } else {
            if (errno == EAGAIN) {
                // Semaphore is not available, introduce a brief delay
                usleep(10000); // 10 milliseconds
            } else {
                // Handle other errors
                perror("sem_trywait error");
            }
        }

        // Check for the terminate flag after each attempt
        if (terminate) {
            break;
        }
    }
    
    //Notify solution was found
    circ_buffer->finished = 1;
    //continue all 
    sem_post(sem_empty);
    sem_post(sem_empty);
    sem_post(sem_empty);
    sem_post(sem_empty);
    sem_post(sem_empty);
    sem_post(sem_empty);
    sem_post(sem_empty);
    sem_post(sem_empty);
    sem_post(sem_empty);
    sem_post(sem_empty);
    sem_close(sem_filled);
    sem_close(sem_empty);
    sem_close(sem_mutex);

    sem_unlink(SEM_FILLED_ID);
    sem_unlink(SEM_EMPTY_ID);
    sem_unlink(SEM_MUTEX_NAME);

    munmap(circ_buffer, sizeof(struct circular_buffer));
    close(opened);

    if (terminate == 1) {
        printf("The graph might not be 3-colorable, best solution removes %d edges.\n", best_solution);
        return EXIT_SUCCESS;
    }
    return 0;
}
