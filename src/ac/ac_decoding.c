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

static uint64_t window;
static uint32_t extra_window;
static int fill_extra = 0;

//shift
static int buffer_full = 0;
static int shift = 9;
static int pending_bits = 0;

void _read_in_32(uint32_t *win);

void _read_endian_32(uint32_t *win) {
    fread(win, sizeof(uint32_t), 1, f_out);
}

void check_extra() {
    if (fill_extra == 8) {
        fprintf(dec_ranges, "\n\nFULL BUFFER shift: %d\n", shift);
        fseek(f_out, shift, SEEK_SET);
        shift++;
        _read_in_32(&extra_window);
        //_read_endian_32(&extra_window);
        fprintf(dec_ranges, "\nNEW EXTRA WINDOW: %s - %X\n\n", int_to_binary(extra_window, 32), extra_window);
        fill_extra = 0;
    }
}

void shift_window_out() {
    fprintf(dec_ranges, "1 -Window before %s %s\n", int_to_binary(window >> 32, 32), int_to_binary(window, 32));
    fprintf(dec_ranges, "1 -Extra before %s\n", int_to_binary(extra_window, 32));
    window = window << 1ULL | 0;
    //extra_window = extra_window << 1 | 0;
    uint32_t tmp_extra = extra_window & 0x80000000U;
    fill_extra++;
    //window = window | (uint64_t)(extra_window>>64);
    //window = window | ((uint64_t) (tmp_extra) >> 32ULL);
    window=window|(uint64_t) tmp_extra>>31;
    extra_window = extra_window << 1 | 0;

    fprintf(dec_ranges, "1 - shift out -Window %s\n", int_to_binary(window >> 32, 32));
    fprintf(dec_ranges, "1 - shift out -Extra After %s\n\n\n", int_to_binary(extra_window, 32));

    check_extra();
}

void init_wa() {
    //window ready loaded 64 bit
    uint32_t win1, win2;
    _read_in_32(&win1);
    fprintf(dec_ranges, "LONG init high %d %s\n", win1, int_to_binary(win1, 32));
    _read_in_32(&win2);
    fprintf(dec_ranges, "LONG init high %d %s\n", win2, int_to_binary(win2, 32));

    //window = ((uint64_t) win1 << 32) | ((uint64_t) win2 & 0xFFFFFFFU);
    window = (uint64_t) win1 << 32 | win2;

    //load added window of 32 bit
    _read_in_32(&extra_window);
    fprintf(dec_ranges, "EXTRA %s\n", int_to_binary(extra_window, 32));
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
    fprintf(dec_ranges, "adjust ranges, underflow bit +%d", underflow_bit);
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
        fprintf(dec_ranges, "read_in_32() %X - %s\n", output, int_to_binary(output, 32));
        buffer_full = 0;
        shift++;
    }
}

void adjust_output_range() {
    for (int i = 0; i < pending_bits; i++) {
        //window<<=1;
        fprintf(dec_ranges, "2 -Window before %s %s\n", int_to_binary(window >> 32, 32), int_to_binary(window, 32));
        fprintf(dec_ranges, "2 -Extra before %s\n", int_to_binary(extra_window, 32));
        uint64_t upper = window & 0x8000000000000000UL;
        uint64_t lower = window & 0x3FFFFFFFFFFFFFFFUL;
        //extra_window=(extra_window&0x80000000)>>31;
        fprintf(dec_ranges, "2 -up - low %s %s\n", int_to_binary(upper >> 32, 32), int_to_binary(lower >> 32, 32));

        uint32_t extra_low = extra_window & 0x80000000;
        fprintf(dec_ranges, "2 -extra low var %s\n", int_to_binary(((extra_low)) >> 31, 32));
        extra_window = extra_window << 1;
        fill_extra++;

        window = (upper) | (lower << 1ULL);
        window = window | ((uint64_t) (extra_low)) >> 31ULL;
        //window = (upper) | (((lower << 1)) | ((uint64_t) (extra_low)) >> 31);
        //window = window|extra_window;

        //CHECK
        //torna indetro 8 byte
        /*fseek(f_out, -1, SEEK_CUR);
        _read_in_32(&extra_window);
        //torna indietro 8 byte
        fseek(f_out, -8, SEEK_CUR);*/

        fprintf(dec_ranges, "2 -Window %s\n", int_to_binary(window >> 32, 32));
        fprintf(dec_ranges, "2 -Extra After %s\n\n\n", int_to_binary(extra_window, 32));


        check_extra();
    }
    /*ADDED AFTER I think it used to work :)
     * for (int k = 0; k < pending_bits; k++) {
        uint32_t upper = output & 0x80000000;

        char *bits_mask = malloc(sizeof(char) * (32));
        bits_mask[0] = '0';
        bits_mask[1] = '0';
        int j = pending_bits;
        for (j = 1; j < 31; j++) {
            bits_mask[j] = '1';
        }
        bits_mask[j] = '\0';
        int mask = binary_to_int(bits_mask, 32);
        fprintf(dec_ranges, "\nthe MASK index %d: %s\n\n", k, int_to_binary(mask, 32));
        uint32_t lower = output & mask;
        fprintf(dec_ranges, "\noutput MASK & %d: %s\n\n", k, int_to_binary(output & mask, 32));

        output = (upper) | (lower << 1);
        fprintf(dec_ranges, "\noutput %d: %s\n\n", k, int_to_binary(output, 32));
        buffer_full++;
        check_full_buffer();
    }
     */
    /*uint32_t upper = output & 0x80000000;
fprintf(dec_ranges, "\nfirst bit&0: %s\n\n", int_to_binary(upper, 32));

char *bits_mask = malloc(sizeof(char) * (32));
bits_mask[0] = '0';
int j;
for (j = 1; j <= pending_bits; j++) {
    bits_mask[j] = '0';
}
for (j = pending_bits; j < 31; j++) {
    bits_mask[j] = '1';
}
bits_mask[j] = '\0';
int mask = binary_to_int(bits_mask, 32);
uint32_t lower = output & mask;

fprintf(dec_ranges, "\nMASK: %s\n\n", int_to_binary(mask, 32));
fprintf(dec_ranges, "\nlower shift bit&0: %s\n\n",
        int_to_binary((upper) | (lower << pending_bits), 32));

output = (upper) | (lower << pending_bits);
fprintf(dec_ranges, "\noutput bit&0: %s %u\n\n", int_to_binary(output, 32), output);
for (int k = 0; k < pending_bits; ++k) {
    buffer_full++;
    check_full_buffer();
}*/
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
      //  print_element_file(p, dec_ranges);
#endif
        if (is_in_range(p, window >> 32)) {
            //fount it and win
            fprintf(dec_ranges, "0 - Window %s %s \t %X %X\n", int_to_binary(window >> 32, 32), int_to_binary(window, 32),
                    (uint32_t)(window >> 32), (uint32_t)(window));
            //this char as been found
            char c = get_element_char(p);
            fwrite(&c, 1, 1, dec_output);

            fprintf(dec_ranges, "The char is: %c\n\n", c);
            //element has high and low
            low = get_element_start(p);
            high = get_element_end(p);
            int size = 32;
            char *low_bin = int_to_binary(low, size);
            char *high_bin = int_to_binary(high, size);

            //are there any bit to out
            char *real_bit_to_out = check_output_range(low_bin, high_bin, size);

            if (strlen(real_bit_to_out) != 0) {
                //adjust_output_range();
                pending_bits = 0;
            }

            if (strlen(real_bit_to_out) == 0) {
                //only increment pending go on
                //adjust range no shift
                fprintf(dec_ranges, "help output=%s\n", int_to_binary(window >> 32, 32));
                char *underflow = underflow_check(int_to_binary(low << 1, 32), int_to_binary(high << 1, 32), 32);
                int underflow_bit = strlen(underflow);
                pending_bits += underflow_bit;

                adjust_output_range();
                adjust_range(underflow_bit);
            }
            else {
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
            fprintf(dec_ranges, "\nOutput:\t%u - %s%s\n\n", window >> 32, int_to_binary(window >> 32, 32), int_to_binary(window,32));
#endif
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
    f_out = fopen("ac_output", "rb");
    dec_output = fopen("dec_output.txt", "w");
#if DEBUG_FILE_PRINT
    dec_ranges = fopen("dec_ranges.txt", "w");
#endif
    for (int i = 0; i < ARRLEN; i++) {
        frequency[i] = frq[i];
    }
}