//
// Created by alice on 08.11.17.
//

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
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
static uint32_t high = 0xFFFFFFFFU;//0x7FFFFFFF;
static int flag_exit = 1; //boolean
static uint32_t output = 0;
static FILE *f_out;

static uint64_t window = 0;
static uint32_t extra_window = 0;
static int fill_extra = 0;

//shift
static int buffer_full = 0;
static int shift = ARRLEN * 4 + 13;//13;//9;
static int pending_bits = 0;

void _read_in_32(uint32_t *win);

void _read_endian_32(uint32_t *win) {
    fread(win, sizeof(uint32_t), 1, f_out);
}

void check_extra() {
    if (fill_extra == 8) {
        fseek(f_out, shift, SEEK_SET);
        shift++;
        _read_in_32(&extra_window);
        fill_extra = 0;
    }
}

void shift_window_out() {
#if DEBUG_FILE_PRINT
    fprintf(dec_ranges, "1 - shift\n");
#endif
    window = window << 1ULL | 0;
    //extra_window = extra_window << 1 | 0;
    uint32_t tmp_extra = extra_window & 0x80000000U;
    fill_extra++;
    window = window | (uint64_t) tmp_extra >> 31;
    extra_window = extra_window << 1 | 0;

    check_extra();
}

void init_wa() {
    //prepare files
    f_out = fopen("ac_output", "rb");
    dec_output = fopen("dec_output.txt", "w");
#if DEBUG_FILE_PRINT
    dec_ranges = fopen("dec_ranges.txt", "w");
#endif
    //read total char
    fread(&total_char, 1, sizeof(uint32_t), f_out);
    printf("Char: %u", total_char);

    //read array ARRLEN
    for (int i = 0; i < ARRLEN; ++i) {
        fread(&frequency[i], sizeof(uint32_t), 1, f_out);
        printf("%d\n", frequency[i]);
    }

    //window ready loaded 64 bit
    uint32_t win1, win2;
    _read_in_32(&win1);
    _read_in_32(&win2);

#if DEBUG_FILE_PRINT
    fprintf(dec_ranges, "win 1: %s", int_to_binary(win1, 32));
    fprintf(dec_ranges, "win 2: %s", int_to_binary(win2, 32));
#endif

    window = (uint64_t) win1 << 32 | win2;

    fseek(f_out, 0, SEEK_END);
    unsigned long fs = ftell(f_out);
    rewind(f_out);
    //fseek(f_out, 8, SEEK_SET);
    //char
    //fseek(f_out, 12, SEEK_SET);
    //array
    fseek(f_out, ARRLEN * 4 + 12, SEEK_SET);
#if DEBUG_FILE_PRINT
    fprintf(dec_ranges, "File read: %d\n", fs);
#endif

    //load added window of 32 bit
    _read_in_32(&extra_window);
}

void _read_in_32(uint32_t *win) {
    fread(win, sizeof(uint32_t), 1, f_out);
    //fread(&output, sizeof(uint32_t), 4, f_out);
    //00011010  00010010  11110110  10000100
    //00011010  00010010  11110110  10000100
    //1000 0100  1111 0110  0001 0010  0001 1010
    //Little endian
    //https://stackoverflow.com/questions/14791349/is-fread-on-a-single-integer-affected-by-the-endianness-of-my-system
    //FORSE CHECK?????????????
    *win = (((*win >> 0) & 0xff) << 24)
            | (((*win >> 8) & 0xff) << 16)
            | (((*win >> 16) & 0xff) << 8)
            | (((*win >> 24) & 0xff) << 0);
}

void read_in_32() {
    fread(&output, sizeof(uint32_t), 1, f_out);
    //fread(&output, sizeof(uint32_t), 4, f_out);
    //00011010  00010010  11110110  10000100
    //00011010  00010010  11110110  10000100
    //1000 0100  1111 0110  0001 0010  0001 1010
    //Little endian
    //https://stackoverflow.com/questions/14791349/is-fread-on-a-single-integer-affected-by-the-endianness-of-my-system
    //FORSE CHECK?????????????
    window = (((window >> 0) & 0xff) << 24)
            | (((window >> 8) & 0xff) << 16)
            | (((window >> 16) & 0xff) << 8)
            | (((window >> 24) & 0xff) << 0);
}

void adjust_range(int underflow_bit) {
    low = (low << underflow_bit);
    low &= ~(1 << 31);
    for (int j = 0; j < underflow_bit; j++) {
        high = (high << 1 | 1);
    }
    high = high | (1 << 31);
}

void check_full_buffer() {
    if (buffer_full == 8) {
        fseek(f_out, shift, SEEK_SET);
        read_in_32();
        buffer_full = 0;
        shift++;
    }
}

void adjust_output_range() {
    for (int i = 0; i < pending_bits; i++) {
#if DEBUG_FILE_PRINT
        fprintf(dec_ranges, "2 - pending shift\n");
#endif
        uint64_t upper = window & 0x8000000000000000UL;
        uint64_t lower = window & 0x3FFFFFFFFFFFFFFFUL;
        uint32_t extra_low = extra_window & 0x80000000;
        extra_window = extra_window << 1;
        fill_extra++;

        window = (upper) | (lower << 1ULL);
        window = window | ((uint64_t) (extra_low)) >> 31ULL;

        check_extra();
    }
}

//https://stackoverflow.com/questions/199333/how-to-detect-integer-overflow
size_t highestBitPosition(uint32_t a) {
    size_t bits = 0;
    while (a != 0) {
        ++bits;
        a >>= 1;
    }
    return bits;
}

void dac_ranges(unsigned char next_char, int i) {
    if (flag_exit) {
        //Create new element
        Element *p = init_element((unsigned char) i, frequency[i]);
        pointer_to_char[n] = p;
        n++;

        //Calculating and updating element ranges
        //Overflow
        size_t a_bits = highestBitPosition(get_element_frequency(p)), b_bits = highestBitPosition((high - low));
        if (a_bits + b_bits <= 32) {
            set_element_range(p, get_element_frequency(p) * (high - low) / total_char);
        } else {
            set_element_range(p, (high - low) / total_char * get_element_frequency(p));
        }
        set_element_points(p, old, old + get_element_range(p));
        old += get_element_range(p);

#if DEBUG_FILE_PRINT
        //print_element_file(p, dec_ranges);
#endif
        if (is_in_range(p, window >> 32)) {
            //this char as been found
            char c = get_element_char(p);
            fwrite(&c, 1, 1, dec_output);
#if DEBUG_FILE_PRINT
            fprintf(dec_ranges, "The char is: %c\n\n", c);
#endif
            //element has high and low
            low = get_element_start(p);
            high = get_element_end(p);
            int size = 32;
            char *low_bin = int_to_binary(low, size);
            char *high_bin = int_to_binary(high, size);

            //are there any bit to out
            char *real_bit_to_out = check_output_range(low_bin, high_bin, size);

            if (strlen(real_bit_to_out) != 0) {
                pending_bits = 0;
            }

            if (strlen(real_bit_to_out) == 0) {
                //only increment pending go on
                //adjust range no shift
#if DEBUG_FILE_PRINT
                fprintf(dec_ranges, "help output: %s\n", int_to_binary(window >> 32, 32));
#endif
                char *underflow = underflow_check(int_to_binary(low << 1, 32), int_to_binary(high << 1, 32), 32);
                int underflow_bit = strlen(underflow);
                pending_bits += underflow_bit;

                adjust_output_range();
                adjust_range(underflow_bit);

                //free
                free(underflow);
            } else {
                //Shift calcolo pending
                uint32_t shifted_low = low;
                uint32_t shifted_high = high;
                for (int j = 0; j < strlen(real_bit_to_out); j++) {
                    shifted_low = shifted_low << 1;
                    shifted_high = (shifted_high << 1) | 1;
                }
                //pending
                char *underflow = underflow_check(int_to_binary(shifted_low << 1, 32),
                                                  int_to_binary(shifted_high << 1, 32), 32);
                int underflow_bit = strlen(underflow);
                pending_bits += underflow_bit;

                //free
                free(underflow);

                //Shift
                for (int j = 0; j < strlen(real_bit_to_out); j++) {
                    low = low << 1;
                    high = (high << 1) | 1;
                    shift_window_out();
                }

                //adjust
                adjust_output_range();
                adjust_range(underflow_bit);
                pending_bits = 0;
            }


            //update to print
            low_bin = int_to_binary(low, size);
            high_bin = int_to_binary(high, size);
#if DEBUG_FILE_PRINT
            fprintf(dec_ranges, "\nBinary rappresentation\n START %s\n END   %s\n\n", low_bin,
                    high_bin);
            fprintf(dec_ranges, "\nLow - High:\t%u - %u\n\n", low, high);
            fprintf(dec_ranges, "\nPending: %d\n\n", pending_bits);
            fprintf(dec_ranges, "\nvar buffer_full:\t%d\n", fill_extra);
            fprintf(dec_ranges, "\nOutput:\t%u - %s%s\n\n", window >> 32, int_to_binary(window >> 32, 32),
                    int_to_binary(window, 32));
#endif
            //free
            free(low_bin);
            free(high_bin);
            free(real_bit_to_out);

            flag_exit = 0;
            return;
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
    /*f_out = fopen("ac_output", "rb");
     dec_output = fopen("dec_output.txt", "w");
 #if DEBUG_FILE_PRINT
     dec_ranges = fopen("dec_ranges.txt", "w");
 #endif*/
    for (int i = 0; i < ARRLEN; i++) {
        frequency[i] = frq[i];
    }
}