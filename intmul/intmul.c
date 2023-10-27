#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

/**
 * Reads input line character for character into char*. Pre-allocates 1024 and dynamically increases if growing is necessary.
 * @return char* The string that is allocated and read from the input
 */
static char* read_line(void);
/**
 * Loops through the whole String and checks if each character is a HEX digit.
 * if not returns false;
 * @param str pointer to char* where the content is immutable
 * @return true or false depending on if the whole char* is a hexdigit or not
 */
static bool is_hex(const char*);

/**
 * Pads the new string with leading 0s up to length new_l
 * @pre new_l needs to be longer than strlen(str)
 * @param str input string to pad
 * @param new_l new length to be padded
 */
static int pad_string(char** , int);

static int pad_string(char **pstr, int new_l) {
    if (pstr == NULL || *pstr == NULL || new_l <= strlen(*pstr)) {
        return 1;
    }

    int original_len = strlen(*pstr);
    int padding_len = new_l - original_len;
    char *new_str = realloc(*pstr, new_l + 1); // +1 for null terminator

    if (!new_str) {
        fprintf(stderr, "Reallocation for string failed\n");
        // No need to free *pstr; original memory remains intact upon realloc failure.
        return 1;
    }

    *pstr = new_str;  // Update the input string pointer

    // Move the characters to the right to create space for padding
    for (int i = original_len; i >= 0; i--) {  // including null terminator
        (*pstr)[i + padding_len] = (*pstr)[i];
    }

    // Fill the new space with zeros
    for (int i = 0; i < padding_len; i++) {
        (*pstr)[i] = '0';
    }
    return 0;
}


static bool is_hex(const char* str) {
    while (*str) {
        if (!isdigit(*str) && !(tolower(*str) >= 'a' && tolower(*str) <= 'f')) {
            return false;
        }
        str++;
    }
    return true;
}

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

    int len1 = strlen(line1);
    int len2 = strlen(line2);

    if (len1 > len2) {
        if (pad_string(&line2, len1) == 1) {
            fprintf(stderr, "Padding string failed for string 2\n");
            free(line1);
            free(line2);
            return EXIT_FAILURE;
        }
    } else if (len2 > len1) {
        if (pad_string(&line1, len2)==1 ) {
            fprintf(stderr, "Padding string failed for string2\n");
            free(line1);
            free(line2);
            return EXIT_FAILURE;
        }
    }

    printf("%s", line1);
    printf("%s", line2);
    free(line1);
    free(line2);
    return 0;
}

