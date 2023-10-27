#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

/**
 * Reads input line character for character into char*. Pre-allocates 1024 and dynamically increases if growing is necessary.
 * @return char* The string that is allocated and read from the input
 */
static char *read_line(void);

/**
 * Loops through the whole String and checks if each character is a HEX digit.
 * if not returns false;
 * @param str pointer to char* where the content is immutable
 * @return true or false depending on if the whole char* is a hexdigit or not
 */
static bool is_hex(const char *);

/**
 * Pads the new string with leading 0s up to length new_l
 * @pre new_l needs to be longer than strlen(str)
 * @param str pointer to input string
 * @param new_l new length to be padded
 */
static int pad_string(char **, size_t);
/**
 * Multiplies both hex values and writes to stdout
 * @pre len of both char arrays is 1
 * @param hex1
 * @param hex2
 */
static void multiply_and_write_stdout(char*, char*);

/**
 * Takes unallocated half strings and allocates first half of input into the first and vice versa with second
 * @pre l != 0
 * @param input Input string that needs to be allocated
 * @param first_h char* that gets allocated and duplicated into
 * @param second_h char* that gets allocated and duplicated into
 * @param l lenth of input needs to be power of 2
 */
static void split_and_insert(char *, char **, char **, size_t);

int main(void) {
    char *line1 = read_line();
    char *line2 = read_line();

    if (is_hex(line1) == false) {
        fprintf(stderr, "Entered wrong input for number 1\n");
        return EXIT_FAILURE;
    }

    if (is_hex(line2) == false) {
        fprintf(stderr, "Entered wrong input for number 2\n");
        return EXIT_FAILURE;
    }

    size_t len1 = strlen(line1);
    size_t len2 = strlen(line2);

    //Base Case for recursion
    if (len1 == 1 && len2 == 1) {
        multiply_and_write_stdout(line1, line2);
        return EXIT_SUCCESS;
    }

    size_t len1Missing = len1 % 2;
    size_t len2Missing = len2 % 2;

    if (pad_string(&line1, len1+len1Missing) == 1 || pad_string(&line2, len2+len2Missing) == 1) {
        fprintf(stderr, "Padding failed\n");
        free(line1);
        free(line2);
        return EXIT_FAILURE;
    }

    len1 = strlen(line1);
    len2 = strlen(line2);

    if (len1 > len2) {
        if (pad_string(&line2, len1) == 1) {
            fprintf(stderr, "Padding string failed for string 2\n");
            free(line1);
            free(line2);
            return EXIT_FAILURE;
        }
    } else if (len2 > len1) {
        if (pad_string(&line1, len2) == 1) {
            fprintf(stderr, "Padding string failed for string2\n");
            free(line1);
            free(line2);
            return EXIT_FAILURE;
        }
    }

    size_t total_l = strlen(line1);
    char* ah;
    char* aj;
    char* bh;
    char* bj;

    split_and_insert(line1, &ah, &aj, total_l);
    split_and_insert(line2, &bh, &bj, total_l);

    char* inputs[4][2] = {{ah, bh}, {ah, bj}, {aj, bh}, {aj, bj}};

    pid_t child_process_ids[4];
    int inPipes[4][2];
    int outPipes[4][2];

    for (int i = 0; i < 4; i++)
    {
        pipe(inPipes[i]);
        pipe(outPipes[i]);
        child_process_ids[i] = fork();

        if (child_process_ids[i] < 0) {
            fprintf(stderr, "Forking failed");
            return EXIT_FAILURE;
        }
        if (child_process_ids[i] == 0) { // Child
            close(inPipes[i][1]);
            close(outPipes[i][0]);

            dup2(inPipes[i][0], STDIN_FILENO);
            dup2(outPipes[i][1], STDOUT_FILENO);

            close(inPipes[i][0]);
            close(outPipes[i][1]);

            execlp("./intmul", "intmul", (char *) NULL);
            exit(1); // execlp failed
        } else { // Parent
            close(inPipes[i][0]);
            close(outPipes[i][1]);

            dprintf(inPipes[i][1], "%s\n%s\n", inputs[i][0], inputs[i][1]);
            close(inPipes[i][1]);
        }
    }

    char buffer[2048];

    unsigned long results[4];

    for (int i = 0; i < 4; i++) {
        memset(buffer, 0, sizeof(buffer));
        read(outPipes[i][0], buffer, sizeof(buffer) - 1);  // -1 null-termination
        results[i] = strtoul(buffer, NULL, 16);
        close(outPipes[i][0]);
    }
    for (int i = 0; i < 4; i++) {
        wait(NULL); // Wait for all children to terminate
    }
    size_t n = total_l / 2;
    unsigned long result =
            (results[0] << (4 * n * 2)) +
            (results[1] << (4 * n)) +
            (results[2] << (4 * n)) +
            results[3];

    printf("%lx",result);

    //freeing of allocated resources
    free(ah);
    free(aj);
    free(bh);
    free(bj);
    free(line1);
    free(line2);
    return 0;
}

static void split_and_insert(char *input, char **first_h, char **second_h, size_t input_l) {
    size_t half_l = input_l / 2;

    *first_h = strndup(input, half_l);
    *second_h = strndup(input + half_l, half_l);
}


static void multiply_and_write_stdout(char* hex1, char* hex2) {
    unsigned long val1 = strtoul(hex1, NULL, 16);
    unsigned long val2 = strtoul(hex2, NULL, 16);

    // Multiply
    unsigned long product = val1 * val2;

    // Print the result
    fprintf(stdout, "%lx\n", product);
}


static int pad_string(char ** input_str, size_t new_l) {
    if (strlen(*input_str) == new_l) {
        return 0;
    }
    if (input_str == NULL || *input_str == NULL || new_l < strlen(*input_str)) {
        return 1;
    }

    size_t original_len = strlen(*input_str);
    size_t padding_len = new_l - original_len;
    *input_str = realloc(*input_str, new_l + 1); // +1 for null terminator

    if (!input_str) {
        fprintf(stderr, "Reallocation for string failed\n");
        return 1;
    }

    // Move the characters to the right to create space for padding
    for (int i = original_len; i >= 0; i--) {  // including null terminator
        (*input_str)[i + padding_len] = (*input_str)[i];
    }

    // Fill the new space with zeros
    for (int i = 0; i < padding_len; i++) {
        (*input_str)[i] = '0';
    }
    return 0;
}

static bool is_hex(const char *str) {
    while (*str) {
        if (!isdigit(*str) && !(tolower(*str) >= 'a' && tolower(*str) <= 'f')) {
            return false;
        }
        str++;
    }
    return true;
}

static char *read_line(void) {
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

