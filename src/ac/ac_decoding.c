//
// Created by alice on 08.11.17.
//

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ac_decoding.h"
#include "element.h"

#define ARRLEN 256

#if DEBUG_FILE_PRINT
FILE *dec_ranges;
#endif
FILE *dec_output;

static int frequency[ARRLEN];
static Element *pointer_to_char[ARRLEN];
static int n = 0;
static int total_char;
static uint32_t old = 0;
static uint32_t low = 0;
static uint32_t high = 0x7FFFFFFF;
static int flag_exit = 1; //boolean
static uint32_t output = 0;
static FILE *f_out;

//shift
static int buffer_full = 0;
static int shift = 1;

void read_in_32() {
    fread(&output, sizeof(uint32_t), 1, f_out);
    //fread(&output, sizeof(uint32_t), 4, f_out);
    //00011010  00010010  11110110  10000100
    //00011010  00010010  11110110  10000100
    //1000 0100  1111 0110  0001 0010  0001 1010
    //Little endian
    //https://stackoverflow.com/questions/14791349/is-fread-on-a-single-integer-affected-by-the-endianness-of-my-system
    //FORSE CHECK?????????????
    output = (((output >> 0) & 0xff) << 24)
            | (((output >> 8) & 0xff) << 16)
            | (((output >> 16) & 0xff) << 8)
            | (((output >> 24) & 0xff) << 0);
}


void dac_ranges(unsigned char next_char, int i) {
    if (flag_exit) {
        //Create new element
        Element *p = init_element((unsigned char) i, frequency[i]);
        pointer_to_char[n] = p;
        n++;

        //Calculating and updating element ranges
        set_element_range(p, get_element_frequency(p) * (high - low) / total_char);
        set_element_points(p, old, old + get_element_range(p));
        old += get_element_range(p);

#if DEBUG_FILE_PRINT
        //print_element_file(p, dec_ranges);
#endif
        if (is_in_range(p, output)) {
#if DEBUG_FILE_PRINT
            fprintf(dec_ranges, "\nCHAR %c\n\n", get_element_char(p));
#endif
            //fprintf(dec_output, "%c", get_element_char(p));
            char c=get_element_char(p);
            fwrite(&c, 1, 1, dec_output);

            low = get_element_start(p);
            high = get_element_end(p);

            char *low_bin = int_to_binary(low, 32);
            char *high_bin = int_to_binary(high, 32);

            char *bit_to_out = check_output_range(low_bin, high_bin, 32);

#if DEBUG_FILE_PRINT
            //Binary rappresentation
            fprintf(dec_ranges, "\nOutput as: %s\n\n", int_to_binary(output, 32));
            fprintf(dec_ranges, "\nBinary rappresentation\n START %s\n END   %s\n\n", low_bin,
                    high_bin);
            fprintf(dec_ranges, "\nEquals bit: %s-\n\n", bit_to_out);
#endif
            //Shift
            for (int j = 0; j < strlen(bit_to_out); j++) {
                low = low << 1;
                high = (high << 1) | 1;

                output = output << 1 | (int) bit_to_out[j];

                buffer_full++;
                if (buffer_full == 8) {
                    fseek(f_out, shift, SEEK_SET);
                    read_in_32();
#if DEBUG_FILE_PRINT
                    fprintf(dec_ranges, "\nFull buffer\": %X %s\n", output, int_to_binary(output, 32));
#endif
                    printf("Hex red_in_32() %X - %s\n", output, int_to_binary(output, 32));
                    buffer_full = 0;
                    shift++;
                }
            }

            low_bin = int_to_binary(low, 32);
            high_bin = int_to_binary(high, 32);
#if DEBUG_FILE_PRINT
            //Binary rappresentation
            fprintf(dec_ranges, "\nShifted output as: %X %s\n\n", output, int_to_binary(output, 32));
            fprintf(dec_ranges, "\nBinary rappresentation\n START %s\n END   %s\n\n", low_bin,
                    high_bin);
#endif
            flag_exit = 0;
        }
    }
}

void ac_decode() {
    for (int k = 0; k < total_char; k++) {
#if DEBUG_FILE_PRINT
        fprintf(dec_ranges, "\n=======================================\n\n");
#endif
        //Part to re-calculate every range
        for (int i = 0; i < ARRLEN; i++) {
            if (frequency[i] != 0) {
                dac_ranges((unsigned char) frequency[i], i);
            }
        }
        flag_exit = 1;
        old = low;
        n = 0;
    }
}

void set_total_char(int t_ch) {
    total_char = t_ch;
}

void set_frequency(int *frq, int len) {
    f_out = fopen("ac_output", "rb");
    dec_output = fopen("dec_output.txt", "w");
#if DEBUG_FILE_PRINT
    dec_ranges = fopen("dec_ranges.txt", "w");
#endif
    for (int i = 0; i < ARRLEN; i++) {
        frequency[i] = frq[i];
    }
}