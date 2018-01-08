//
// Created by alice on 08.11.17.
//

#include <stdio.h>
#include <stdint.h>
#include <string.h>;
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

//shift
static int buffer_full = 0;
static int shift = 1;

static int pending_bits = 0;

//static int flag_executed = 0;

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

void adjust_output_range(){
    uint32_t upper = output & 0x80000000;
    fprintf(dec_ranges, "\nfirst bit&0: %s\n\n", int_to_binary(upper, 32));
    //output=output>>1|1;

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
    uint32_t lower = output & mask;//0x0fffffff;
    fprintf(dec_ranges, "\nMASK: %s\n\n", int_to_binary(mask, 32));
    fprintf(dec_ranges, "\nlower shift bit&0: %s\n\n",
            int_to_binary((upper) | (lower << pending_bits), 32));
    output = (upper) | (lower << pending_bits);
    fprintf(dec_ranges, "\noutput bit&0: %s %u\n\n", int_to_binary(output, 32), output);
    for (int k = 0; k <pending_bits; ++k) {
        buffer_full++;
        check_full_buffer();
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
        print_element_file(p, dec_ranges);
#endif
        if (is_in_range(p, output)) {
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

            //bit to out in case of pending and bit_to_out
            char *bit_to_out_pending_case = check_output_range(int_to_binary(low << 1, size),
                                                               int_to_binary(high << 1, size), size);

            if(strlen(real_bit_to_out)!=0){
                pending_bits = 0;
            }

            if (strlen(real_bit_to_out) == 0) {
                //only increment pending go on
                //adjust range no shift
                fprintf(dec_ranges, "help\n");
                char *underflow = underflow_check(int_to_binary(low << 1, 32), int_to_binary(high << 1, 32), 32);
                int underflow_bit = strlen(underflow);
                pending_bits += underflow_bit;
                adjust_output_range();
                adjust_range(underflow_bit);
            }
                //tolto else{
            else {
                uint32_t shifted_low = low;
                uint32_t shifted_high = high;
                //Shift
                for (int j = 0; j < strlen(real_bit_to_out); j++) {
                    shifted_low = shifted_low << 1;
                    shifted_high = (shifted_high << 1) | 1;
                }
                //pending
                char *underflow = underflow_check(int_to_binary(shifted_low << 1, 32),
                                                  int_to_binary(shifted_high << 1, 32), 32);
                int underflow_bit = strlen(underflow);
                pending_bits += underflow_bit;
                //shift out pending
                if (pending_bits != 0) {
                    /*int count=0;
                    fprintf(dec_ranges, "\nBefore output:\t%u - %s count: %d\n\n", output, int_to_binary(output, 32), count);
                    if (real_bit_to_out[0] == 0) {
                        output = output << 1 | 0;
                    }
                    else {
                        output = output << 1 | 1;
                    }
                    count++;
                    buffer_full++;
                    check_full_buffer();

                    fprintf(dec_ranges, "\nMSB output:\t%u - %s +bit %d count: %d\n\n", output, int_to_binary(output, 32), real_bit_to_out[0]-'0', count);
                    for (int j = 0; j < pending_bits; j++) {
                        if(real_bit_to_out[0] == '0') {
                            output = output << 1 | 1;
                        }
                        else{
                            output = output << 1 | 0;
                        }
                        count++;
                        buffer_full++;
                        check_full_buffer();
                    }
                    fprintf(dec_ranges, "\ntotal pending output:\t%u - %s count: %d\n\n", output, int_to_binary(output, 32), count);
                    for (int j = 0; j < strlen(bit_to_out_pending_case); j++) {
                        output = (output << 1) | bit_to_out_pending_case[j] - '0';
                        count++;
                        buffer_full++;
                        check_full_buffer();
                    }
                    fprintf(dec_ranges, "\nFinal output:\t%u - %s count: %d\n\n", output, int_to_binary(output, 32), count);*/
                }

                int check = 0;
                //Shift
                for (int j = 0; j < strlen(real_bit_to_out); j++) {
                    low = low << 1;
                    high = (high << 1) | 1;
                    if (pending_bits == 0) {
                        output = (output << 1) | (real_bit_to_out[j] - '0');
                        buffer_full++;
                        check_full_buffer();
                        check = 0;
                    } else {
                        output = (output << 1) | (real_bit_to_out[j] - '0');
                        buffer_full++;
                        check_full_buffer();
                        check = 1;
                    }
                }
                /*if(check){
                    for (int j = 0; j < strlen(real_bit_to_out)+1; j++) {
                        output = (output << 1) | (real_bit_to_out[j] - '0');
                        buffer_full++;
                        check_full_buffer();
                    }
                }*/
                if (pending_bits != 0) {
                    adjust_output_range();
                }

                /*output = (output << underflow_bit);
                output &= ~(1 << 31);*/

                //adjust
                adjust_range(underflow_bit);
            }

            /*if(pending_bits!=0){
                //fprintf(dec_ranges, "\noutput shift <<%u\n", pending_bits);
                //output=output<<pending_bits;
                //mask out 2-pendingbits
                fprintf(dec_ranges, "\noutput before <%s>\n", int_to_binary(output, 32));
                //output=output<<(pending_bits);
                uint32_t upper = output & 0xC0000000;
                fprintf(dec_ranges, "\nUPPER shift out <%s>\n", int_to_binary(upper, 32));
                //output=output>>1;
                uint32_t lower = output & 0x1FFFFFFF;
                //output=(upper) | (lower<<1);
                //output=output|0x80000000;
                //output = output & (0xFFFFFFFFU >> 1);
                //output = output & (0xFFFFFFFFU >> 1);
                fprintf(dec_ranges, "\nLOWER shift in <%s>\n", int_to_binary(lower, 32));
                fprintf(dec_ranges, "\noutput shift in <%s>\n", int_to_binary(output, 32));
            }*/

            //update to print
            low_bin = int_to_binary(low, size);
            high_bin = int_to_binary(high, size);
#if DEBUG_FILE_PRINT
            fprintf(dec_ranges, "\nBinary rappresentation\n START %s\n END   %s\n\n", low_bin,
                    high_bin);
            fprintf(dec_ranges, "\nLow - High:\t%u - %u\n\n", low, high);
            fprintf(dec_ranges, "\nPending: %d\n\n", pending_bits);
            fprintf(dec_ranges, "\nvar buffer_full:\t%d\n", buffer_full);
            fprintf(dec_ranges, "\nOutput:\t%u - %s\n\n", output, int_to_binary(output, 32));
#endif
            //add
            //pending_bits = 0;

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