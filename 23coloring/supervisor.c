#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <limits.h>
#include <semaphore.h>
#include <sys/stat.h>
#include "shared.h"

static void read_solutions(circular_buffer *buffer) {
    if (buffer->nr_in_use > 0) {
        solution sol = buffer->solutions[buffer->start];
        buffer->start = (buffer->start + 1) % 10;
        buffer->nr_in_use--;

        // Process the solution
        // For example, print it or perform calculations
    } else {}//do nothing and wait
}

int main(int argc, char *argv[]) {
    int limit = INT_MAX;
    int delay = 0;
    char* intparsecheck;

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


    printf("%d", limit);
    printf("%d", delay);


    int opened = shm_open("/12124528", O_EXCL | O_RDWR, 0600);

    if (opened == -1) {
        fprintf(stderr, "Failed call to shm_open");
        return EXIT_FAILURE;
    }

    if(ftruncate(opened, sizeof(circular_buffer)) == -1 ) {
        fprintf(stderr, "Failed to truncate shared memory");
        return EXIT_FAILURE;
    }

    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED;
    circular_buffer *circ_buffer = mmap(0, sizeof(circular_buffer), protection, visibility,opened,0);

    if (circ_buffer == MAP_FAILED) {
        fprintf(stderr, "Failed to call mmap()");
        return EXIT_FAILURE;
    }

    circ_buffer->start = 0;
    circ_buffer->end = 0;
    circ_buffer->nr_in_use = 0;

    while (1) {

    }
    munmap(circ_buffer, sizeof(circular_buffer));
    close(opened);
    return 0;
}
