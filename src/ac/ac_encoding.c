//
// Created by alice on 10.10.17.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "element.h"

#define ARRLEN 256
//#define uint32_t int

#if DEBUG_FILE_PRINT
//File print Log Debug
FILE *f_freq;
//File ac_ranges every step Log Debug
FILE *f_enc;
#endif
//File that will contain the final output
FILE *f_output;

//Array frequency
static int frequency[ARRLEN];

//Array Element pointers, it will
//contain every element with specific
//ranges and attributes each iteration
static Element *pointer_to_char[ARRLEN];

//total encoded chars
static int total_char = 0;
//temp variable to keep old value
//high of preceeding range in ac_ranges
static uint32_t old = 0;

//temp variable to pass through
//element in pointer_to_char
static int n = 0;

// integer 32 bit from 0 - 0111 1111 1111 1111 1111 1111 1111 1111
// (next will be full 1000 0000 0000 0000 0000 0000 0000 0000)
static uint32_t low = 0;
static uint32_t high = 0xFFFFFFFFU;

//Buffer output to file
static char buffer[9] = "33333333";

//Count char to EOF
static int counter_to_EOF = 0;

//Total of pending bits
static int pending_bits = 0;

/**
 * Manages output to file
 * writing 8 bit from buffer
 * @param bit_to_out
 */
void write_8_bit_to_out(char *bit_to_out) {
    int buf_index = 0;
    for (int i = 0; i < strlen(bit_to_out); i++) {
        if (buf_index == 8) {
            //buf char to buf int
            long l_bin = binary_to_int(buffer, 32);
            fwrite(&l_bin, 1, 1, f_output);
            char ptr[strlen(bit_to_out) - i];
            strncpy(ptr, &bit_to_out[i], strlen(bit_to_out) - i + 1);
            strcpy(buffer, "33333333");
            write_8_bit_to_out(ptr);
            return;
        }

        if (buffer[buf_index] == '3') {
            buffer[buf_index] = bit_to_out[i];
            buf_index++;
        } else {
            buf_index++;
            i--;
        }

    }
}

/**
 * This funtion execute only once at the end
 * of the compression:
 * write the low integer
 * empty the buffer
 */
void ac_end() {
    //fill buffer with low val
    write_8_bit_to_out(int_to_binary(low, 32));
    //out last bit in buffer
    for (int i = 0; i < 8; i++) {
        if (!(buffer[i] != '3')) {
            buffer[i] = '0';
        }
    }
    printf("Last writing---\n");
    long l_bin = binary_to_int(buffer, 32);
    fwrite(&l_bin, 1, 1, f_output);

    fclose(f_output);
}

/**
 *
 * @return int *frequency
 */
int *get_frequency() {
    return (frequency);
}

/**
 * This function determinates the index of the highest
 * bit that is a 1 in the a integer
 * @param a is the integer where I'm looking for highest 1
 * @return
 */
size_t highestOneBitPosition(uint32_t a) {
    size_t bits = 0;
    while (a != 0) {
        ++bits;
        a >>= 1;
    }
    return bits;
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

    //Overflow checking
    size_t a_bits = highestOneBitPosition(get_element_frequency(p)), b_bits = highestOneBitPosition((high - low));
    if (a_bits + b_bits <= 32) {
        set_element_range(p, get_element_frequency(p) * (high - low) / total_char);
    } else {
        set_element_range(p, (high - low) / total_char * get_element_frequency(p));
    }
    set_element_points(p, old, old + get_element_range(p));
    old += get_element_range(p);

#if DEBUG_FILE_PRINT
    //print_element_file(p, f_enc);
#endif
}

/**
 *
 * @param next_char the char to be encoded
 */
void ac_encode(unsigned char next_char) {
    for (int i = 0; i < n; i++) {
        if (get_element_char(pointer_to_char[i]) == next_char) {

#if DEBUG_FILE_PRINT
            fprintf(f_enc, "Encoding: %c\n", next_char);
#endif
            low = get_element_start(pointer_to_char[i]);
            high = get_element_end(pointer_to_char[i]);

            int size = 32;

            //Update
            char *low_bin;
            char *high_bin;
            if (pending_bits != 0) {
                low_bin = int_to_binary(low << 1, size);
                high_bin = int_to_binary(high << 1, size);
            } else {
                low_bin = int_to_binary(low, size);
                high_bin = int_to_binary(high, size);
            }

            //Binary bit that don't change this will be sent out
            //and ranges will update
            char *real_bit_to_out = check_output_range(low_bin, high_bin, size);

            //every 8 bit
            if (pending_bits == 0) {
                write_8_bit_to_out(real_bit_to_out);
            }

            //free
            free(low_bin);
            free(high_bin);

            low_bin = int_to_binary(low, size);
            high_bin = int_to_binary(high, size);

            char *bit_to_out = check_output_range(low_bin, high_bin, size);
            if (strlen(bit_to_out) == 0) {
#if DEBUG_FILE_PRINT
                fprintf(f_enc, "help");
#endif
                //no output
                //increment pending
            } else {
#if DEBUG_FILE_PRINT
                fprintf(f_enc, "\nEquals bit: ");
#endif

                if (pending_bits != 0) {
                    char *o = "1";
                    char *z = "0";
                    if (bit_to_out[0] == '0') {
                        //converge to low
#if DEBUG_FILE_PRINT
                        fprintf(f_enc, "0");
#endif
                        write_8_bit_to_out(z);
                        for (int k = 0; k < pending_bits; k++) {
                            char *o = "1";
#if DEBUG_FILE_PRINT
                            fprintf(f_enc, "%s", o);
#endif
                            write_8_bit_to_out(o);
                        }
                    } else {
                        //converge to high
#if DEBUG_FILE_PRINT
                        fprintf(f_enc, "1");
#endif
                        write_8_bit_to_out(o);
                        for (int k = 0; k < pending_bits; k++) {
#if DEBUG_FILE_PRINT
                            fprintf(f_enc, "%s", z);
#endif
                            write_8_bit_to_out(z);
                        }
                    }
                    write_8_bit_to_out(real_bit_to_out);
                    //write_8_bit_to_out(bit_to_out);
                    pending_bits = 0;
                }
#if DEBUG_FILE_PRINT
                fprintf(f_enc, "%s", real_bit_to_out);
#endif
            }

#if DEBUG_FILE_PRINT
            //Binary rappresentation
            fprintf(f_enc, "\nBinary rappresentation\n START %s\n END   %s\n\n", low_bin,
                    high_bin);
#endif

            //Shift
            for (int j = 0; j < strlen(bit_to_out); j++) {
                low = low << 1;
                high = (high << 1) | 1;
            }
            //aggiunto if
            if (strlen(bit_to_out) != 0) {
                if (pending_bits != 0) {
                    low = low << 1;
                    high = (high << 1) | 1;
                }
            }

            char *underflow = underflow_check(int_to_binary(low << 1, 32), int_to_binary(high << 1, 32), 32);
            uint32_t underflow_bit = strlen(underflow);
            pending_bits += underflow_bit;

            low = (low << underflow_bit);
            low &= ~(1 << 31);
            for (int j = 0; j < underflow_bit; j++) {
                high = (high << 1 | 1);
            }
            high = high | (1 << 31);

            //free
            free(low_bin);
            free(high_bin);

            low_bin = int_to_binary(low, size);
            high_bin = int_to_binary(high, size);
#if DEBUG_FILE_PRINT
            //Binary rappresentation
            fprintf(f_enc, "\nBinary rappresentation\n START %s\n END   %s\n\n", low_bin,
                    high_bin);
            fprintf(f_enc, "\nLow - High:\t%u - %u\n\n", low, high);
            fprintf(f_enc, "\nPending: %d\n\n", pending_bits);
            fprintf(f_enc, "\n--------------------------------------------------------\n");
#endif

            //free low and high pointer
            free(low_bin);
            free(high_bin);
            free(real_bit_to_out);
            free(underflow);
        }
    }


    old = low;
    n = 0;

#if DEBUG_FILE_PRINT
    if (low > high) {
        fprintf(f_enc, "\nwe have a problem\n");
    }
#endif

    //free pointer?
    for (int i = 0; i < ARRLEN; i++) {
        free_element(pointer_to_char[i]);
        pointer_to_char[i] = 0;
    }

    //Part to re-calculate every range
    for (int i = 0; i < ARRLEN; i++) {
        if (frequency[i] != 0) {
            ac_ranges((unsigned char) frequency[i], i);
        }
    }

    if ((counter_to_EOF + 1) == total_char) {
        counter_to_EOF++;
        ac_encode(next_char);

#if DEBUG_FILE_PRINT
        fprintf(f_enc, "END WRITE");
        fprintf(f_enc, "\nBinary rappresentation\n START %s\n END   %s\n\n", int_to_binary(low, 32),
                int_to_binary(high, 32));
#endif
        ac_end();
    }
    counter_to_EOF++;
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
    f_output = fopen("ac_output", "wb");
    //char sul file
    fwrite(&total_char, 1, sizeof(uint32_t), f_output);

    //array frequenze file
    fwrite(frequency, sizeof(uint32_t), ARRLEN, f_output);
#if DEBUG_FILE_PRINT
    f_freq = fopen("f_freq.txt", "w+");
    f_enc = fopen("ac_ranges.txt", "w+");
    for (int i = 0; i < sizeof(frequency) / sizeof(int); i++) {
        fprintf(f_freq, "%c = %u\n", (char) i, frequency[i]);
    }
    fclose(f_freq);
#endif
}