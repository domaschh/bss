#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hexCharToInt(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

char intToHexChar(int n) {
    if (n >= 0 && n <= 9) return '0' + n;
    if (n >= 10 && n <= 15) return 'A' + (n - 10);
    return '0';
}

// Function to perform a bitwise left shift on a hex string.
char* shiftHexLeftByBits(const char* hex, size_t bits) {
    size_t hexLen = strlen(hex);
    size_t totalBits = hexLen * 4;
    size_t newTotalBits = totalBits + bits;
    size_t newHexLen = (newTotalBits + 3) / 4;

    char* shifted = (char*)calloc(newHexLen + 1, sizeof(char));
    if (!shifted) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize shifted hex string with '0'.
    memset(shifted, '0', newHexLen);

    // Start shifting from the end of the hex string.
    for (int i = hexLen - 1; i >= 0; i--) {
        int value = hexCharToInt(hex[i]);
        for (int j = 0; j < 4; j++) {
            if (value & (1 << j)) {
                int bitPos = (hexLen - i - 1) * 4 + j + bits; // Calculate the bit position after shift
                if (bitPos < newTotalBits) {
                    shifted[bitPos / 4] |= 1 << (bitPos % 4);
                }
            }
        }
    }

    // Convert shifted binary values back to hexadecimal characters.
    for (size_t i = 0; i < newHexLen; i++) {
        int digitValue = 0;
        for (int j = 0; j < 4; j++) {
            if (shifted[i] & (1 << j)) {
                digitValue |= 1 << j;
            }
        }
        shifted[i] = intToHexChar(digitValue);
    }

    // Reverse the string since we built it in reverse.
    for (size_t i = 0; i < newHexLen / 2; i++) {
        char temp = shifted[i];
        shifted[i] = shifted[newHexLen - i - 1];
        shifted[newHexLen - i - 1] = temp;
    }

    return shifted;
}

void reverseString(char* str) {
    int i = 0;
    int j = strlen(str) - 1;
    while (i < j) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

char* addHexStrings(const char* hex1, const char* hex2) {
    // Find lengths of the hex strings
    int len1 = strlen(hex1);
    int len2 = strlen(hex2);
    // The maximum length of the result would be one more than the longest input
    int maxLen = (len1 > len2 ? len1 : len2) + 1;

    // Allocate space for the result (including null terminator)
    char* result = calloc(maxLen + 1, sizeof(char));
    if (!result) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    int carry = 0;
    // Start adding from the least significant digit (end of the string)
    for (int i = 0; i < maxLen; i++) {
        int digit1 = (i < len1) ? hexCharToInt(hex1[len1 - 1 - i]) : 0;
        int digit2 = (i < len2) ? hexCharToInt(hex2[len2 - 1 - i]) : 0;
        int sum = digit1 + digit2 + carry;

        // Update the carry
        carry = sum / 16;
        // Convert the sum to a single hex digit
        result[maxLen - 1 - i] = intToHexChar(sum % 16);
    }

    // If the last carry is nonzero, shift the result to make space for it
    if (result[0] == '0') {
        // Move the string one place to the right, discarding the leading '0'
        memmove(result, result + 1, maxLen);
    }

    return result;
}

int main() {
    const char* hex = "1A";
    size_t bits = 0;

    char* shifted = shiftHexLeftByBits(hex, bits);

    printf("Original: %s\n", hex);
    printf("\n--------------\n");
    printf("Shifted : %s\n", shifted);

    unsigned long test = strtoul(hex, NULL, 16);
    unsigned long s = test << bits;
    printf("Shifted real: %lX\n", s);
    char* result = addHexStrings("1A", "1A");
    unsigned long r = strtoul("1A", NULL, 16) + strtoul("1A", NULL, 16);
    printf("\n--------------\n");
    printf("Result real: %lu\n", r);
    printf("Result func: %s\n", result);
    printf("\n--------------\n");
    free(shifted);

    return 0;
}
