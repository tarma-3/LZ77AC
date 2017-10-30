//
// Created by alice on 10.10.17.
//
#include <stdio.h>
#include "element.h"

#if DEBUG_FILE_PRINT
FILE *file;
FILE *f_enc;
#endif

static int frequency[255];
static Element *pointer_to_char[255];

static double total_char = 0;
static double old = 0;
static double low = 0;
static double high = 1;
static int n = 0;

int *get_frequency() {
    return (frequency);
}

void ac_ranges(unsigned char next_char, int i) {
    Element *p = init_element((unsigned char) i, frequency[i]);
    pointer_to_char[n] = p;
    n++;

    set_element_range(p, get_element_frequency(p) * (high - low) / total_char);
    set_element_points(p, old, old + get_element_range(p));

    old += get_element_range(p);

    print_element_file(p, f_enc);
}

void ac_encode(unsigned char next_char) {
    for (int i = 0; i < n; i++) {
        //printf("%c ", get_element_char(pointer_to_char[i]));
        if (get_element_char(pointer_to_char[i]) == next_char) {
            fprintf(f_enc, "\nNext CHAR:\t%c\n\n", next_char);
            low = get_element_start(pointer_to_char[i]);
            high = get_element_end(pointer_to_char[i]);
            fprintf(f_enc, "\nLow - High:\t%lf - %lf\n\n", low, high);
        }
    }
    fprintf(f_enc, "\n=======================================\n\n");

    old = low;
    n = 0;

    fprintf(f_enc, "\nOld as low:\t%lf\n\n", old);
    //memset(pointer_to_char, 0, sizeof(pointer_to_char));

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

void print_frequency() {
#if DEBUG_FILE_PRINT
    file = fopen("logPrint.txt", "w+");
    f_enc = fopen("ac_ranges.txt", "w+");
    for (int i = 0; i < sizeof(frequency) / sizeof(int); i++) {
        fprintf(file, "%c = %u\n", (char) i, frequency[i]);
    }
    fclose(file);
#endif
}