//
// Created by alice on 10.10.17.
//
#include <stdio.h>

int frequency[128];

static unsigned int low = 0;
static unsigned int high = 0xFFFFFFFFU;
static double output = 0;

void ac_encode(unsigned char next_char) {
    int range = high - low;

    printf("%d=\t%c\n", frequency[next_char], (char)next_char);
    high = low + range * frequency[next_char];
    low = low + range * frequency[next_char];

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
}

void print_frequency() {
    for (int i = 0; i < sizeof(frequency) / sizeof(int); i++) {
        printf("%c = %u\n", (char) i, frequency[i]);
    }
}