//
// Created by alice on 08.11.17.
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "ac_decoding.h"
#include "element.h"

#if DEBUG_FILE_PRINT
FILE *dec_ranges;
#endif






// ** WORK IN PROGRESS **
// This code DOES NOT WORK
// ** it just contains some code ideas **












static int total_char;
static uint32_t low = 0;
//********************************
static uint32_t high = 0x7FFFFFFF+1;
static uint32_t old = 0;
//********************************
static uint32_t output = 1073741824;

void dac_ranges(int *frequency, uint32_t output) {
    for (int i = 0; i < 255; i++) {
        if (frequency[i] != 0) {
            Element *p = init_element((unsigned char) i, frequency[i]);

            set_element_range(p, get_element_frequency(p) * (high - low) / total_char);
            //printf("%d\n", get_element_range(p));
            set_element_points(p, old, old + get_element_range(p));
            old += get_element_range(p);
#if DEBUG_FILE_PRINT
            //fprintf(dec_ranges, "\nNext CHAR:\t%c\n\n", next_char);
            print_element_file(p, dec_ranges);
            if (is_in_range(p, output)) {
                fprintf(dec_ranges, "\nCHAR %c\n\n", get_element_char(p));
                low = get_element_start(p);
                high = get_element_end(p);
            }
#endif
        }
    }
#if DEBUG_FILE_PRINT
    fprintf(dec_ranges, "\nLow - High:\t%u - %u\n\n", low, high);
#endif
}

//Frequency ?
void ac_decode(int *frq) {
    for (int i = 0; i < 5; i++) {
        // ???? output?
        dac_ranges(frq, output);
        output = (low + high) / 2;
#if DEBUG_FILE_PRINT
        fprintf(dec_ranges, "\nNEXT OUTPUT:\t%u \n\n", output);
        fprintf(dec_ranges, "\n=======================================\n\n");
#endif
    }
}

void set_total_char(int t_ch) {
    total_char = t_ch;
}

int *set_frequency(int *frq, int len) {
#if DEBUG_FILE_PRINT
    dec_ranges = fopen("dec_ranges.txt", "w");
#endif
    int *frequency = malloc(len * sizeof(int));
    memcpy(frequency, frq, len * sizeof(int));
    return frequency;
}