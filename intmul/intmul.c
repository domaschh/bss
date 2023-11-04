
/**
 * @file intmul.c
 * @author Thomas Mages 12124528
 * @brief Intmul Implementation
 * @details This program recursively multiplies two number
 * @date 18-10-2023
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/**
 * Reads input line character for character into char*. Pre-allocates 1024 and dynamically increases if growing is necessary.
 * @details fails if the line is '\0' or
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
static int split_and_insert(char *, char **, char **, size_t);

/**
 * Converts a Hex char to int
 * considers both Uppercase and lowercase
 * @param c
 * @return
 */
static int hex_char_to_int(char);

/**
 * Converts an integer to a Hex character
 * @param n the integer to be converted
 * @return  char that is returned
 */
static char int_to_hex_char(int n);


/**
 * Mimics the << operator. Creates a new char* that has the shifted hex value inside
 * @param hex char* that holds the hex value
 * @param bits
 * @return
 */
static char* leftshift_hex_str(const char*, size_t);

/**
 * Adds two hex numbers represented as char* and returns the result as another char*
 * @param number1
 * @param number2
 * @return
 */
static char* add_hex_str(char* number1, char* number2);

int main(void) {
    char *line1 = read_line();
    if (strlen(line1) == 0) {
        fprintf(stderr, "Expected two lines of input. Received only one or none.\n");
        free(line1);
        return EXIT_FAILURE;
    }

    char *line2 = read_line();
    if (strlen(line2) == 0) {
        fprintf(stderr, "Expected two lines of input. Received only one.\n");
        free(line1);
        free(line2);
        return EXIT_FAILURE;
    }
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

    if (split_and_insert(line1, &ah, &aj, total_l) == -1) {
        fprintf(stderr, "Splitting line one failed");
        return EXIT_FAILURE;
    }

    if (split_and_insert(line2, &bh, &bj, total_l) == -1) {
        fprintf(stderr, "Splitting line one failed");
        return EXIT_FAILURE;
    }

    char* inputs[4][2] = {{ah, bh}, {ah, bj}, {aj, bh}, {aj, bj}};

    pid_t child_process_ids[4];
    int in_pipes[4][2];
    int out_pipes[4][2];

    for (int i = 0; i < 4; i++)
    {
        if (pipe(in_pipes[i]) == -1) {
            fprintf(stderr, "Piping inputs failed");
            return EXIT_FAILURE;
        }
        if (pipe(out_pipes[i]) == -1) {
            fprintf(stderr, "Piping outputs failed");
            return EXIT_FAILURE;
        }

        child_process_ids[i] = fork();

        if (child_process_ids[i] < 0) {
            fprintf(stderr, "Forking failed");
            return EXIT_FAILURE;
        }
        if (child_process_ids[i] == 0) {
            if (dup2(in_pipes[i][0], STDIN_FILENO) == -1) {
                fprintf(stderr, "Dup2 failed for in_pipes");
                return EXIT_FAILURE;
            }
            if(dup2(out_pipes[i][1], STDOUT_FILENO) == -1) {
                fprintf(stderr, "Dup2 failed for outipes");
                return EXIT_FAILURE;
            }

            close(in_pipes[i][0]);
            close(out_pipes[i][1]);
            close(in_pipes[i][1]);
            close(out_pipes[i][0]);

            execlp("./intmul", "intmul", (char *) NULL);
            exit(EXIT_FAILURE); // execlp failed
        } else { // Parent
            close(in_pipes[i][0]);
            close(out_pipes[i][1]);

            dprintf(in_pipes[i][1], "%s\n%s\n", inputs[i][0], inputs[i][1]);
            close(in_pipes[i][1]);
        }
    }

    for (int i = 0; i < 4; i++) {
        int child_status;
        waitpid(child_process_ids[i], &child_status, 0);
        if (child_status == 1) {
            fprintf(stderr, "Waiting for child failed");
            return EXIT_FAILURE;
        }
    }

    char buffer[2048];

    char* results[4];

    for (int i = 0; i < 4; i++) {
        memset(buffer, 0, sizeof(buffer));
        read(out_pipes[i][0], buffer, sizeof(buffer) - 1);  // -1 null-termination
        results[i] = strdup(buffer);
        close(out_pipes[i][0]);
    }
    
    size_t n = total_l / 2;
    char* s1 = leftshift_hex_str(results[0], 4*n*2);
    char* s2 = leftshift_hex_str(results[1], 4*n);
    char* s3 = leftshift_hex_str(results[2], 4*n);
    char* s4 = leftshift_hex_str(results[3], 0);
    char* tmp1 = add_hex_str(s1, s2);
    char* tmp2 = add_hex_str(s3, s4);
    char* tmp3 = add_hex_str(tmp1, tmp2);

    printf("%s",tmp3);

    free(results[0]);
    free(results[1]);
    free(results[2]);
    free(results[3]);
    free(s1);
    free(s2);
    free(s3);
    free(s4);
    free(tmp1);
    free(tmp2);
    free(tmp3);
    free(ah);
    free(aj);
    free(bh);
    free(bj);
    free(line1);
    free(line2);
    return 0;
}

static int split_and_insert(char *input, char **first_h, char **second_h, size_t input_l) {
    size_t half_l = input_l / 2;

    *first_h = strndup(input, half_l);
    if (first_h == NULL) {
        return - 1;
    }
    *second_h = strndup(input + half_l, half_l);
    if (second_h == NULL) {
        return - 1;
    }
    return 0;
}


static void multiply_and_write_stdout(char* hex1, char* hex2) {
    unsigned long val1 = strtoul(hex1, NULL, 16);
    unsigned long val2 = strtoul(hex2, NULL, 16);
    fprintf(stdout, "%lx", val1 * val2);
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

    if (input_str == NULL) {
        fprintf(stderr, "Reallocation for string failed\n");
        return EXIT_FAILURE;
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
    int buffer_size = 1024;
    int position = 0;
    char *buffer = malloc(sizeof(char) * buffer_size);
    char curr_char;

    if (!buffer) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (true) {
        curr_char = fgetc(stdin);
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


static int hex_char_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

static char int_to_hex_char(int n) {
    if (n >= 0 && n <= 9) return '0' + n;
    if (n >= 10 && n <= 15) return 'a' + (n - 10);
    return '0';
}

static char* leftshift_hex_str(const char* hex, size_t bits) {
    size_t hexDigitsToAdd = bits / 4;
    size_t len = strlen(hex);
    size_t newLen = len + hexDigitsToAdd;

    char* shifted = calloc(newLen + 1, sizeof(char));
    if (!shifted) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    strcpy(shifted, hex);

    memset(shifted + len, '0', hexDigitsToAdd);

    return shifted;
}

static char* add_hex_str(char* number1, char* number2) {
    size_t len1 = strlen(number1);
    size_t len2 = strlen(number2);
    // The maximum length can be one more than the longest input (overflow)
    size_t maxLen = (len1 > len2 ? len1 : len2) + 1;

    //(including null terminator)
    char* result = calloc(maxLen + 1, sizeof(char));

    if (!result) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    int carry = 0;
    // Start adding from the least significant digit (end of the string)
    for (int i = 0; i < maxLen; i++) {
        int digit1 = (i < len1) ? hex_char_to_int(number1[len1 - 1 - i]) : 0;
        int digit2 = (i < len2) ? hex_char_to_int(number2[len2 - 1 - i]) : 0;
        int sum = digit1 + digit2 + carry;

        // Update the carry
        carry = sum / 16;
        // Convert the sum to a single hex digit
        result[maxLen - 1 - i] = int_to_hex_char(sum % 16);
    }

    // If the last carry is nonzero, shift the result to make space for it
    if (result[0] == '0') {
        // Move the string one place to the right, discarding the leading '0'
        memmove(result, result + 1, maxLen);
    }

    return result;
}

