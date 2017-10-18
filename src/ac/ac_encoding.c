//
// Created by alice on 10.10.17.
//
#include <stdio.h>
#include <stdlib.h>

#if DEBUG_FILE_PRINT
FILE *file;
#endif

static int frequency[255];

static int total_char = 0;
static double low = 0;
static double high = 1;
static double output = 0;

int *get_frequency() {
    return (frequency);
}

void ac_encode(unsigned char next_char) {

    printf("\t%c\t", (char) next_char);
    double range = high - low;
    printf("%d=\t%c\n", frequency[next_char], (char) next_char);
    high = low + range * frequency[next_char] / total_char;
    low = low + range * frequency[next_char] / total_char;

    output = low + (high - low) / 2;
    printf("%lf\n", output);
}

/**
 *
 * @param next_char char to calculate for the frequency array
 */
void build_frequency(unsigned char next_char) {
    printf("%c\n", next_char);
    //Following ASCII table
    frequency[next_char]++;
    total_char++;
}

void print_frequency() {
#if DEBUG_FILE_PRINT
    file = fopen("logPrint.txt", "w+");
    for (int i = 0; i < sizeof(frequency) / sizeof(int); i++) {
        fprintf(file, "%c = %u\n", (char) i, frequency[i]);
    }
    fclose(file);
#endif
}