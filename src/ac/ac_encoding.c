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

//** IN PROGRESS **
//changing low and high to be
// integer 32 bit from 0 - 0111 1111 1111 1111 1111 1111 1111 1111
// (next will be full 1000 0000 0000 0000 0000 0000 0000 0000)
static uint32_t low = 0;
static uint32_t high = 0xFFFFFFFFU;//0x7FFFFFFF;

//Buffer output to file
static char buffer[9] = "33333333";

//Count char to EOF
static int counter_to_EOF = 0;

static int pending_bits = 0;

/**
 *
 * @return int total_char that indicates all char encoded
 */
int get_total_char() {
    return (total_char);
}

/**
 * Manages output to file
 * writing 8 bit from buffer
 * @param bit_to_out
 */
void write_8_bit_to_out(char *bit_to_out) {
    int buf_index = 0;
    printf("To write in %s\n", bit_to_out);
    for (int i = 0; i < strlen(bit_to_out); i++) {
        if (buf_index == 8) {
            //printf("BUFFER write: %s", buffer);
            //buf char to buf int
            long l_bin = binary_to_int(buffer, 32);
            //printf("%li ", l_bin);
            fwrite(&l_bin, 1, 1, f_output);
            char ptr[strlen(bit_to_out) - i];
            //strncpy(ptr, &bit_to_out[i], sizeof(bit_to_out)-i);
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
    for (int i = 0; i < 8; i++) {
        printf("%c ", buffer[i]);
    }
    printf("\n------\n");
}

void ac_end() {
    printf("\nEND\n");
    //fill buffer with low val
    printf("%s ", int_to_binary(low, 32));
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

    //long intval=8;
    //fwrite(&intval, 1, 1, f_output);
    fclose(f_output);
}

/**
 *
 * @return int *frequency
 */
int *get_frequency() {
    return (frequency);
}

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

    //Calculating and updating element ranges
    //fprintf(f_enc, "\nhigh %u", high);
    //funzionava
    /*if(calc) {
        set_element_range(p, get_element_frequency(p) * (high - low) / total_char);
    }
    else{
        set_element_range(p, get_element_frequency(p) * (high - low) / total_char);
        calc=1;
    }*/

    //Overflow
    size_t a_bits = highestOneBitPosition(get_element_frequency(p)), b_bits = highestOneBitPosition((high - low));
    if (a_bits + b_bits <= 32) {
        set_element_range(p, get_element_frequency(p) * (high - low) / total_char);
    } else {
        set_element_range(p, (high - low) / total_char * get_element_frequency(p));
    }

    //fprintf(f_enc, "\tVAL %u\n", (high-low)/total_char*get_element_frequency(p));
    //printf("%d\n", get_element_range(p));
    set_element_points(p, old, old + get_element_range(p));
    old += get_element_range(p);

#if DEBUG_FILE_PRINT
    //print_element_file(p, f_enc);
#endif

    //writing output file FINAL output
    //f_output = fopen("ac_output", "wb");
    //fprintf(f_output, "%d", (low + high) / 2);
    // fprintf(f_output, "%s ", int_to_binary(low, 32));
}

/**
 *
 * @param next_char the char to be encoded
 */
void ac_encode(unsigned char next_char) {
    for (int i = 0; i < n; i++) {
        if (get_element_char(pointer_to_char[i]) == next_char) {

            fprintf(f_enc, "Encoding: %c\n", next_char);
            //add
            //uint32_t half = (low + high) / 2;
            //fprintf(f_enc, "Half: %s - %d\n", int_to_binary(half, 32), half);

            //????

            low = get_element_start(pointer_to_char[i]);
            high = get_element_end(pointer_to_char[i]);

            //qui

            int size = 32;

            //char *low_bin = int_to_binary(low, size);
            //char *high_bin = int_to_binary(high, size);

            /*if (high < half) {
                fprintf(f_enc, "\n*******Write in 0 - %s\n", int_to_binary(high, 32));
                char *z = "0";
                write_8_bit_to_out(z);
                low = low << 1;
                high = high << 1;
            } else if (low >= half) {
                fprintf(f_enc, "\n*******Write in 1 - %s\n", int_to_binary(high, 32));
                char *o = "1";
                write_8_bit_to_out(o);
                low = (low - half) << 1;
                high = (high - half) << 1;
            }*/
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
            if(pending_bits==0){
                write_8_bit_to_out(real_bit_to_out);
            }

            low_bin = int_to_binary(low, size);
            high_bin = int_to_binary(high, size);

            char *bit_to_out = check_output_range(low_bin, high_bin, size);
            if(strlen(bit_to_out)==0){
                fprintf(f_enc, "help");
                //no output
                //increment pending
            }
            else {

                /*if(strlen(bit_to_out)!=0){
                    write_8_bit_to_out(real_bit_to_out);
                }*/

                fprintf(f_enc, "\nEquals bit: ");

                if (pending_bits != 0) {
                    char *o = "1";
                    char *z = "0";
                    if (bit_to_out[0] == '0') {
                        //converge to low
                        fprintf(f_enc, "0");
                        write_8_bit_to_out(z);
                        for (int k = 0; k < pending_bits; k++) {
                            char *o = "1";
                            fprintf(f_enc, "%s", o);
                            write_8_bit_to_out(o);
                        }
                    } else {
                        //converge to high
                        fprintf(f_enc, "1");
                        write_8_bit_to_out(o);
                        for (int k = 0; k < pending_bits; k++) {
                            fprintf(f_enc, "%s", z);
                            write_8_bit_to_out(z);
                        }
                    }
                    write_8_bit_to_out(real_bit_to_out);
                    //write_8_bit_to_out(bit_to_out);
                    pending_bits = 0;
                }

                fprintf(f_enc, "%s", real_bit_to_out);
            }
            //fwrite(buffer, 8, 1, f_output);
            //fprintf(f_output, bit_to_out);

#if DEBUG_FILE_PRINT
            //Binary rappresentation
            fprintf(f_enc, "\nBinary rappresentation\n START %s\n END   %s\n\n", low_bin,
                    high_bin);
#endif
            //if (low < half && high > half) {
            /*fprintf(f_enc, "\n%s - %s\n", int_to_binary(low & 0x80000000, 32), int_to_binary(high & 0x80000000, 32));
            if ((low & 0x80000000) != (high & 0x80000000)) {
                fprintf(f_enc, "\nMiddle\n");
                char *underflow = underflow_check(int_to_binary(low << 1, 32), int_to_binary(high << 1, 32), 32);
                uint32_t underflow_bit = strlen(underflow);
                fprintf(f_enc, "\nUnderflow bit: %s %d\n\n", underflow, underflow_bit);*/

            /*char *o="1";
            char *z="0";

            write_8_bit_to_out(z);
            write_8_bit_to_out(o);*/

            //calc = 0;

            //CLR buffer
            /*fprintf(f_enc, "\nBuffer CLR: %s\n\n", buffer);
            for (int j = 0; j < 8; j++) {
                if (!(buffer[j] != '3')) {
                    buffer[j] = '0';
                }
            }
            printf("CLR BUF---\n");
            long l_bin = binary_to_int(buffer, 32);
            fwrite(&l_bin, 1, 1, f_output);*/

            /*pending_bits+=underflow_bit;
            //write_8_bit_to_out(int_to_binary(low, 32));

            low = (low << underflow_bit);
            low &= ~(1 << 31);
            for (int j = 0; j < underflow_bit; j++) {
                high = (high << 1 | 1);
            }
            high = high | (1 << 31);*/
            //}

            /*if(low>=0x7FFFFFFF/2){
                printf("top half");
            }*/

            /*
             * WORK
             *
             * if (low < half && high > half) {
                fprintf(f_enc, "\nMiddle\n");
                char *underflow = underflow_check(int_to_binary(low << 1, 32), int_to_binary(high << 1, 32), 32);
                int underflow_bit = strlen(underflow);
                fprintf(f_enc, "\nUnderflow bit: %s %d\n\n", underflow, underflow_bit);
                low=(low<<underflow_bit);
                low &= ~(1 << 31);
                for (int j = 0; j < underflow_bit; ++j) {
                    high=(high<<1|1);
                }
                high=high| (1 << 31);
            }*/
            //??SDFsdf??

            //Shift
            for (int j = 0; j < strlen(bit_to_out); j++) {
                low = low << 1;
                high = (high << 1) | 1;
                //write_8_bit_to_out(pending_bits);
            }
            //aggiunto if
            if(strlen(bit_to_out)!=0) {
                if (pending_bits != 0) {
                    low = low << 1;
                    high = (high << 1) | 1;
                }
            }

            char *underflow = underflow_check(int_to_binary(low << 1, 32), int_to_binary(high << 1, 32), 32);
            uint32_t underflow_bit = strlen(underflow);
            pending_bits += underflow_bit;
            //write_8_bit_to_out(int_to_binary(low, 32));

            low = (low << underflow_bit);
            low &= ~(1 << 31);
            for (int j = 0; j < underflow_bit; j++) {
                high = (high << 1 | 1);
            }
            high = high | (1 << 31);

            /*if(strlen(bit_to_out)!=0){
                if(pending_bits!=0){
                    fprintf(f_enc, "\nSHIFT IN Underflow, write to out: ");
                    if(bit_to_out[0]=='0'){
                        //converge to low
                        for(int k=0; k<pending_bits; k++){
                            char *o="1";
                            printf("h1 ");
                            fprintf(f_enc, "%s", o);
                            write_8_bit_to_out(o);
                        }
                    }
                    else{
                        //converge to high
                        for(int k=0; k<pending_bits; k++){
                            char *z="0";
                            printf("h0 ");
                            fprintf(f_enc, "%s", z);
                            write_8_bit_to_out(z);
                        }
                    }
                    pending_bits=0;
                }
            }*/

            /*
            //if(low<half && high>half){
                //Write out all
                //restart ranges
                fprintf(f_enc, "UNDERFLOW");
                uint32_t res=low;
                //fwrite(&res, 1, 1, f_output);
                //clr buf
                //strcpy(buffer, "33333333");
                low = 0;
                high = 0x7FFFFFFF;
            }*/

            /*
             *
             *
             *
             *
             *
             * WORK
             */
            /*
            //add
            uint32_t oldHigh=high;
            uint32_t oldLow=low;
            uint32_t half=(oldHigh+oldLow)/2;
            fprintf(f_enc, "Half: %d - %s\n", half, int_to_binary(half, 32));
            if (high>((oldHigh+oldLow)/2) && low<((oldHigh+oldLow)/2)) {
                fprintf(f_enc, "UNDERFLOW");
            }
            else if(low>=((oldHigh+oldLow)/2)){

                low = 0;
                high = 0x7FFFFFFF;
            }*/

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
            //printf("COUNTER:\t%d - %d\n", counter_to_EOF, total_char);
            /*if((counter_to_EOF+1)==total_char){
                fprintf(f_enc, "END WRITE");
                //ac_end();
            }*/
            //counter_to_EOF++;

            //free low and high pointer
            free(low_bin);
            free(high_bin);
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

    //printf("COUNTER:\t%d - %d\n", counter_to_EOF, total_char);
    if ((counter_to_EOF + 1) == total_char) {
        counter_to_EOF++;
        ac_encode(next_char);

        fprintf(f_enc, "END WRITE");
        fprintf(f_enc, "\nBinary rappresentation\n START %s\n END   %s\n\n", int_to_binary(low, 32),
                int_to_binary(high, 32));

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
#if DEBUG_FILE_PRINT
    f_freq = fopen("f_freq.txt", "w+");
    f_enc = fopen("ac_ranges.txt", "w+");
    for (int i = 0; i < sizeof(frequency) / sizeof(int); i++) {
        fprintf(f_freq, "%c = %u\n", (char) i, frequency[i]);
    }
    fclose(f_freq);
#endif
}

void set_to_counter_EOF() {
    counter_to_EOF++;
}