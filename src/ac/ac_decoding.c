//
// Created by alice on 08.11.17.
//

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ac_decoding.h"
#include "element.h"

#if DEBUG_FILE_PRINT
FILE *dec_ranges;
#endif
FILE *dec_output;

static int frequency[255];
static Element *pointer_to_char[255];
static int n = 0;
static int total_char;
static uint32_t old = 0;
static uint32_t low = 0;
static uint32_t high = 0x7FFFFFFF;
//static uint32_t coso = 0x7FFFFFFF;
//static long long output = 13998411916;
static int flag_exit = 1; //boolean
static uint32_t output = 0;
static FILE *f_out;

void read_in_32() {
    fread(&output, sizeof(uint32_t), 1, f_out);
    //00011010  00010010  11110110  10000100
    //00011010  00010010  11110110  10000100
    //1000 0100  1111 0110  0001 0010  0001 1010
    //Little endian
    //FORSE CHECK?????????????
    output=(((output >> 0) & 0xff) << 24)
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
        print_element_file(p, dec_ranges);
#endif
        if (is_in_range(p, output)) {
#if DEBUG_FILE_PRINT
            fprintf(dec_ranges, "\nCHAR %c\n\n", get_element_char(p));
            fprintf(dec_output, "%c ", get_element_char(p));
#endif
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
            fprintf(dec_ranges, "\nEquals bit: %s\n\n", bit_to_out);
#endif
            //Shift
            for (int j = 0; j < strlen(bit_to_out); j++) {
                low = low << 1;
                high = (high << 1) | 1;
                //shitf bit_to_out out of output????
                //fseek(f_out, strlen(bit_to_out), SEEK_SET);

                output = output << strlen(bit_to_out);
                //read_in_32();


                //OUT EVERY 8 BIT
                //output += bit_to_out[j];
                fprintf(dec_ranges, "\noutput: %li\n", output);
            }

            low_bin = int_to_binary(low, 32);
            high_bin = int_to_binary(high, 32);
            //Binary rappresentation
            fprintf(dec_ranges, "\nShifted output as: %s\n\n", int_to_binary(output, 32));
            fprintf(dec_ranges, "\nBinary rappresentation\n START %s\n END   %s\n\n", low_bin,
                    high_bin);
            flag_exit = 0;

            printf("\nLow: %s + %d\n\n", low_bin, low);
        }
    }
}
/*
void dac_ranges(int *frequency, uint32_t output) {
    for (int i = 0; i < 255; i++) {
        if (frequency[i] != 0) {
            Element *p = init_element((unsigned char) i, frequency[i]);

            print_element(p);
            printf("CALC: %d * (%d - %d) / %d\n",get_element_frequency(p),high_val,low_val,total_char);
            //Calculating and updating element ranges
            set_element_range(p, get_element_frequency(p) * (high_val - low_val) / total_char);
            set_element_points(p, old, old + get_element_range(p));
            old += get_element_range(p);
#if DEBUG_FILE_PRINT
            print_element_file(p, dec_ranges);
#endif
            if (is_in_range(p, output)) {
#if DEBUG_FILE_PRINT
                fprintf(dec_ranges, "\nCHAR %c\n\n", get_element_char(p));
                fprintf(dec_output, "%c ", get_element_char(p));
#endif
                low_val = get_element_start(p);
                high_val = get_element_end(p);

                char *low_bin = int_to_binary(low_val, 32);
                char *high_bin = int_to_binary(high_val, 32);

                char *bit_to_out = check_output_range(low_bin, high_bin, 32);

#if DEBUG_FILE_PRINT
                //Binary rappresentation
                fprintf(dec_ranges, "\nBinary rappresentation\n START %s\n END   %s\n\n", low_bin,
                        high_bin);
                fprintf(dec_ranges, "\nEquals bit: %s\n\n", bit_to_out);
#endif
                //Shift
                for (int j = 0; j < strlen(bit_to_out); j++) {
                    low_val=low_val<<1;
                    high_val=(high_val<<1)|1;
                    //shitf bit_to_out out of output????
                    output=output>>1;
                    //output += bit_to_out[j];
                    fprintf(dec_ranges, "\noutput: %d\n", output);
                }

                low_bin = int_to_binary(low_val, 32);
                high_bin = int_to_binary(high_val, 32);
                //Binary rappresentation
                fprintf(dec_ranges, "\nBinary rappresentation\n START %s\n END   %s\n\n", low_bin,
                        high_bin);
            }
        }
    }
#if DEBUG_FILE_PRINT
    fprintf(dec_ranges, "\nLow - High:\t%u - %u\n\n", low_val, high_val);
#endif
}*/

//Frequency ?
void ac_decode() {
    //read_in_32();
    for (int k = 0; k < total_char; k++) {
#if DEBUG_FILE_PRINT
        //fprintf(dec_ranges, "\nNEXT OUTPUT:\t%u \n\n", output);
        fprintf(dec_ranges, "\n=======================================\n\n");
#endif
        // ???? output?
        //Part to re-calculate every range
        for (int i = 0; i < 255; i++) {
            if (frequency[i] != 0) {
                dac_ranges((unsigned char) frequency[i], i);
            }
        }
        flag_exit = 1;
        //output = (low + high) / 2;
        //output=output<<1;
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
    /*int *frequency = malloc(len * sizeof(int));
    memcpy(frequency, frq, len * sizeof(int));
    return frequency;*/
    for (int i = 0; i < 255; i++) {
        frequency[i] = frq[i];
        //printf("freq: %c %d\n ", (char)i, frequency[i]);
    }
}