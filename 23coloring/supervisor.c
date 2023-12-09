#include "shared.h"

volatile sig_atomic_t terminate = 0;

void handle_signal(int sig) {
    fprintf(stderr,"\nSomething went wrong\n");
    terminate = 1;
}

int main(int argc, char *argv[]) {
    int limit = INT_MAX;
    int delay = 0;
    char* intparsecheck;
    if (sem_unlink(SEM_FILLED_ID) == -1) {
        perror("Error unlinking /sem_filled");
    }
    if (sem_unlink(SEM_EMPTY_ID) == -1) {
        perror("Error unlinking /sem_empty");
    }
    if (sem_unlink(SEM_MUTEX_NAME) == -1) {
        perror("Error unlinking /sem_mutex");
    }
    int opt;

    while ((opt = getopt(argc, argv, "n:w:")) != -1) {
        switch (opt) {
            case 'n':
                limit = strtoul(optarg, &intparsecheck, 10);
                if (*intparsecheck != '\0') {
                    fprintf(stderr, "Invalid number for -n: %s\n", optarg);
                    return EXIT_FAILURE;
                }
                break;
            case 'w':
                delay = strtoul(optarg, &intparsecheck, 10);
                if (*intparsecheck != '\0') {
                    fprintf(stderr, "Invalid number for -n: %s\n", optarg);
                    return EXIT_FAILURE;
                }
                break;
            default: /* '?' */
                fprintf(stderr, "Usage: %s [-n limit] [-w delay]\n", argv[0]);
                return EXIT_FAILURE;
        }
    }


    printf("\n Lim: %d", limit);
    printf("\n delay: %d \n", delay);

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

    wait(delay);

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

    while (terminate == 0) {
        if (solutions_read == limit) {
            terminate = 1;
            break;
        }
    // Attempt to acquire the semaphore without blocking
        if (sem_trywait(sem_filled) == 0) {
            // Semaphore acquired, lock the buffer access
            sem_wait(sem_mutex);

            // Read a solution from the buffer
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

    sem_close(sem_filled);
    sem_close(sem_empty);
    sem_close(sem_mutex);

    sem_unlink(SEM_FILLED_ID);
    sem_unlink(SEM_EMPTY_ID);
    sem_unlink(SEM_MUTEX_NAME);

    munmap(circ_buffer, sizeof(struct circular_buffer));
    close(opened);
    fprintf(stdout,"\n Success Siging \n");

    if (terminate == 1) {
        printf("The graph might not be 3-colorable, best solution removes %d edges.\n", best_solution);
        return EXIT_SUCCESS;
    }
    return 0;
}
