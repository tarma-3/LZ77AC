//
// Created by alice on 25.10.17.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "element.h"

//Struct of each element
//using this struct with *double* instead uint32_t
//it used to work with very simple files
//because of the double capacity

typedef struct element {
    unsigned char c;
    uint32_t start;
    uint32_t end;
    int frequency;
    uint32_t range;
} Element;

Element *init_element(unsigned char c, int fr) {
    Element *el;
    el = malloc(sizeof(Element));
    el->c = c;
    el->frequency = fr;
    return el;
}

unsigned char get_element_char(Element *el) {
    return el->c;
}

void free_element(Element *el) {
    free(el);
}

void print_element(Element *el) {
    printf("Char: %c\n", el->c);
    printf("Frequency: %u\n", el->frequency);
    printf("Range: %u\n", el->range);
    printf("Start: %u\n", el->start);
    printf("End: %u\n\n", el->end);
}

void print_element_file(Element *el, FILE *f) {
    fprintf(f, "Char: %c\n", el->c);
    fprintf(f, "Frequency: %u\n", el->frequency);
    fprintf(f, "Range: %u\n", el->range);
    fprintf(f, "Start: %u\n", el->start);
    fprintf(f, "End: %u\n\n", el->end);
}

int get_element_frequency(Element *el) {
    return el->frequency;
}

uint32_t get_element_start(Element *el) {
    return el->start;
}

uint32_t get_element_end(Element *el) {
    return el->end;
}

uint32_t get_element_range(Element *el) {
    return el->range;
}

void set_element_range(Element *el, uint32_t range) {
    el->range = range;
}

void set_element_points(Element *el, uint32_t start, uint32_t end) {
    el->start = start;
    el->end = end;
}

void set_element_frequency(Element *el, int frq) {
    el->frequency = frq;
}

/**
 * This function will show
 * the range as binary
 *
 * https://stackoverflow.com/questions/699968/display-the-binary-representation-of-a-number-in-c
 */
char *int_to_binary(unsigned int val, char *buff, int sz) {
    //char array to store result
    char *pbuff = buff;

    //Lo zero
    if (val == 0) {
        int i;
        for (i = 0; i < sz; i++) {
            pbuff[i] = '0';
        }
        pbuff[i + 1] = '\0';
    }

    /* Work from the end of the buffer back. */
    pbuff += sz;
    *pbuff-- = '\0';

    /* For each bit (going backwards) store character. */
    for (int i = sz; i > 0; i--) {
        *pbuff-- = ((val & 1) == 1) ? '1' : '0';
        //next bit.
        val >>= 1;
    }
    return pbuff + 1;
}

int is_in_range(Element *el, uint32_t value) {
    return value >= el->start && value < el->end ? 1 : 0;
}

char *check_output_range(char *low, char *high, int size) {
    //Persistente nel tempo
    //printf("%s\n",low);
    //printf("%s\n",low);

    char *to_output = (char *) malloc(size);
    int j = 0;

    for (int i = 0; i < size; i++) {
        if (low[i] == high[i]) {
            to_output[j] = low[i];
            j++;
        } else {
            to_output[++j] = '\0';
            return to_output;
        }
    }
    return to_output;
}