#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

static char* read_line(void);
static bool is_hex(const char*);

/**
 * Loops through the whole String and checks if each character is a HEX digit.
 * if not returns false;
 * @param str pointer to char* where the content is immutable
 * @return true or false depending on if the whole char* is a hexdigit or not
 */
static bool is_hex(const char* str) {
    while (*str) {
        if (!isdigit(*str) && !(tolower(*str) >= 'a' && tolower(*str) <= 'f')) {
            return false;
        }
        str++;
    }
    return true;
}


/**
 * Reads input line character for character into char*. Pre-allocates 1024 and dynamically increases if growing is necessary.
 * @return char* The string that is allocated and read from the input
 */
static char* read_line(void) {
    int buffer_size = 1024;  // Initial buffer size
    int position = 0;
    char *buffer = malloc(sizeof(char) * buffer_size);
    int curr_char;

    if (!buffer) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (true) {
        curr_char = getchar();
        if (curr_char == EOF || curr_char == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = curr_char;
        }
        position++;

        if (position >= buffer_size) {
            buffer_size *= 2;
            buffer = realloc(buffer, buffer_size);
            if (!buffer) {
                fprintf(stderr, "Memory allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

int main(void) {
    char *line1 = read_line();
    char *line2 = read_line();

    printf("You entered:\n%s\n%s\n", line1, line2);
    if (is_hex(line1) == false) {
        fprintf(stderr, "Entered wrong input for number 1\n");
        return EXIT_FAILURE;
    }

    if (is_hex(line2) == false) {
        fprintf(stderr, "Entered wrong input for number 2\n");
        return EXIT_FAILURE;
    }
    free(line1);
    free(line2);
    return 0;
}
