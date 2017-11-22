//
// Created by alice on 10.10.17.
//
#include <stdio.h>
#include "element.h"

#if DEBUG_FILE_PRINT
//File print Log Debug
FILE *f_freq;
//File ac_ranges every step Log Debug
FILE *f_enc;
#endif
//File that will contain the final output
FILE *f_output;

//Array frequency
static int frequency[255];

//Array Element pointers, it will
//contain every element with specific
//ranges and attributes each iteration
static Element *pointer_to_char[255];

//total encoded chars
static int total_char = 0;
//temp variable to keep old value
//high of preceeding range in ac_ranges
static uint32_t old = 0;

//temp variable to pass through
//element in pointer_to_char
static int n = 0;

//** IN PROGRESS **
//changing low and high to be
// integer 32 bit from 0 - 0111 1111 1111 1111 1111 1111 1111 1111
// (next will be full 1000 0000 0000 0000 0000 0000 0000 0000)
static uint32_t low = 0;
static uint32_t high = 0x7FFFFFFF;

/**
 *
 * @return int total_char that indicates all char encoded
 */
int get_total_char() {
    return (total_char);
}

/**
 *
 * @return int *frequency
 */
int *get_frequency() {
    return (frequency);
}

/**
 * Function that update/create the new ranges
 * for the element passed
 *
 * @param next_char, the char to create new/update range
 * @param i position char inside array of frequencies
 */
void ac_ranges(unsigned char next_char, int i) {
    //Create new element
    Element *p = init_element((unsigned char) i, frequency[i]);
    pointer_to_char[n] = p;
    n++;

    //Calculating and updating element ranges
    set_element_range(p, get_element_frequency(p) * (high - low) / total_char);
    //printf("%d\n", get_element_range(p));
    set_element_points(p, old, old + get_element_range(p));
    old += get_element_range(p);

#if DEBUG_FILE_PRINT
    print_element_file(p, f_enc);
#endif

    //writing output file
    f_output = fopen("ac_output", "w");
    fprintf(f_output, "%d", (low + high) / 2);
}

/**
 *
 * @param next_char the char to be encoded
 */
void ac_encode(unsigned char next_char) {
    for (int i = 0; i < n; i++) {
        if (get_element_char(pointer_to_char[i]) == next_char) {
            low = get_element_start(pointer_to_char[i]);
            high = get_element_end(pointer_to_char[i]);
#if DEBUG_FILE_PRINT
            fprintf(f_enc, "\nNext CHAR:\t%c\n\n", next_char);
            fprintf(f_enc, "\nLow - High:\t%u - %u\n\n", low, high);

            int size = 32;
            char bufSt[size + 1];
            char bufEn[size + 1];

            char *lowBin = int_to_binary(low, bufSt, size);
            char *highBin = int_to_binary(high, bufEn, size);

            fprintf(f_enc, "\nBinary rappresentation\n START %s\n END   %s\n\n", lowBin,
                    highBin);

            fprintf(f_enc, "\nEquals bit: %s\n", check_output_range(lowBin, highBin, size));
#endif
        }
    }


    old = low;
    n = 0;

#if DEBUG_FILE_PRINT
    fprintf(f_enc, "\n=======================================\n\n");
    fprintf(f_enc, "\nOld as low:\t%u\n\n", old);
#endif

    //Part to recalculate every range
    for (int i = 0; i < 255; i++) {
        if (frequency[i] != 0) {
            ac_ranges((unsigned char) frequency[i], i);
        }
    }
}

/**
 *
 * @param next_char char to calculate for the frequency array
 */
void build_frequency(unsigned char next_char) {
    //printf("%c\n", next_char);
    //Following ASCII table
    frequency[next_char]++;
    total_char++;
}

/**
 * Print to Log Debug files
 * init debug files
 */
void print_frequency() {
#if DEBUG_FILE_PRINT
    f_freq = fopen("f_freq.txt", "w+");
    f_enc = fopen("ac_ranges.txt", "w+");
    for (int i = 0; i < sizeof(frequency) / sizeof(int); i++) {
        fprintf(f_freq, "%c = %u\n", (char) i, frequency[i]);
    }
    fclose(f_freq);
#endif
}