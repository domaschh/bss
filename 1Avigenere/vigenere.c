/**
 * @file vigenere.c
 * @author Thomas Mages 12124528
 * @brief Vigenere Cipher Implementation
 * @details This program implements the Vigenere cipher. It allows encryption
 * and decryption based on a provided key. It can handle input directly from
 * stdin or from a file and can output to stdout or a specified file.
 * @date 18-10-2023
 */
#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET_SIZE 26

/**
 * @brief Processes the input data with Vigenere Cipher.
 * @param input Input file stream.
 * @param output Output file stream.
 * @param key Cipher key.
 * @param decrypt Flag to determine if decryption is needed.
 */
static void en_or_decrypt(FILE *, FILE *, char *, bool);

/**
 * @brief Parse command-line arguments.
 * @param argc Argument count.
 * @param argv Argument values.
 * @param decrypt Pointer to flag for decryption.
 * @param key Pointer to cipher key string.
 * @param outfile Pointer to output file name.
 * @param fileStartIndex Pointer to start index for file arguments.
 * @return Returns true if arguments parsed successfully, otherwise false.
 */
static bool parse_args(int, char **, bool *, char **, char **, int *);

int main(int argc, char *argv[]) {
    bool decrypt = false;
    char *key = NULL;
    char *outfile = NULL;
    int fileStartIndex;

    if (!parse_args(argc, argv, &decrypt, &key, &outfile, &fileStartIndex)) {
        return EXIT_FAILURE;
    }

    // Validate the key
    for (int i = 0; key[i]; i++) {
        if (!isalpha(key[i])) {
            fprintf(stderr, "Error: Key must contain only alphabet characters.\n");
            return EXIT_FAILURE;
        }
    }

    FILE *output = stdout;
    if (outfile) {
        output = fopen(outfile, "w");
        if (!output) {
            perror("Error opening output file");

            return EXIT_FAILURE;
        }
    }

    if (fileStartIndex >= argc) { // No file arguments, use stdin
        en_or_decrypt(stdin, output, key, decrypt);
    } else {
        for (int i = fileStartIndex; i < argc; i++) {
            FILE *input = fopen(argv[i], "r");
            if (!input) {
                perror("Error opening input file");
                if (outfile) {
                    fclose(output);
                }
                return EXIT_FAILURE;
            }
            en_or_decrypt(input, output, key, decrypt);
            fclose(input);
        }
    }

    if (outfile) {
        fclose(output);
    }

    return EXIT_SUCCESS;
}

static void en_or_decrypt(FILE *input, FILE *output, char *key, bool decrypt) {
    int keyLen = strlen(key);
    int keyIndex = 0;
    int currentChar;

    while ((currentChar = fgetc(input)) != EOF) {
        // Reset key index on newline
        if (currentChar == '\n') {
            keyIndex = 0;
            fputc(currentChar, output);
            continue;
        }
        char keyChar = tolower(key[keyIndex % keyLen]) - 'a';

        // If uppercase character
        if (isupper(currentChar)) {
            if (decrypt) {
                currentChar =
                        ((currentChar - 'A') - keyChar + ALPHABET_SIZE) % ALPHABET_SIZE +
                        'A';
            } else {
                currentChar = ((currentChar - 'A') + keyChar) % ALPHABET_SIZE + 'A';
            }
            fputc(currentChar, output);
        }
            // If lowercase character
        else if (islower(currentChar)) {
            if (decrypt) {
                currentChar = ((currentChar - 'a') - keyChar + ALPHABET_SIZE) % ALPHABET_SIZE +
                              'a';
            } else {
                currentChar = ((currentChar - 'a') + keyChar) % ALPHABET_SIZE + 'a';
            }
            fputc(currentChar, output);
        }
            // Other characters remain unchanged
        else {
            fputc(currentChar, output);
        }
        keyIndex = (keyIndex + 1) % keyLen;
    }
}

static bool parse_args(int argc, char *argv[], bool *decrypt, char **key, char **outfile, int *fileStartIndex) {
    int opt;
    int dCount = 0; // Counter for -d flag

    *decrypt = false;
    *outfile = NULL;

    while ((opt = getopt(argc, argv, "do:")) != -1) {
        switch (opt) {
            case 'd':
                *decrypt = true;
                dCount++;
                if (dCount > 1) {
                    fprintf(stderr, "Error: Multiple -d options are not allowed.\n");
                    return false;
                }
                break;
            case 'o':
                *outfile = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s [-d] [-o outfile] key [file...]\n", argv[0]);
                return false;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Expected key after options.\n");
        return false;
    }

    *key = argv[optind];
    *fileStartIndex = optind + 1;

    return true;
}
