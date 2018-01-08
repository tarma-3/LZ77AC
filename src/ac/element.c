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
    uint32_t frequency;
    uint32_t range;
} Element;

Element *init_element(unsigned char c, uint32_t fr) {
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

void set_element_frequency(Element *el, uint32_t frq) {
    el->frequency = frq;
}

/**
 *
 * @param val value to convert to binary
 * @param sz size of integer value
 * @return char * pointer to space containg char rappresentation of
 * binary value
 */
char *int_to_binary(unsigned int val, int sz) {
    char *ptr_to_char=malloc(sizeof(char)*(sz-1));
    int mask = 1;
    mask = mask << (sz-1);
    int i;
    for (i = 0; i <= sz; i++) {
        ptr_to_char[i]=(((val & mask) == 0) ? '0' : '1');
        val = val << 1;
    }
    ptr_to_char[sz]='\0';
    return ptr_to_char;
}

int is_in_range(Element *el, uint32_t value) {
    return value >= el->start && value < el->end ? 1 : 0;
}

char *check_output_range(char *low, char *high, int size) {
    //Persistente nel tempo

    char *to_output = (char *) malloc(size);
    int j = 0;

    for (int i = 0; i < size; i++) {
        if (low[i] == high[i]) {
            to_output[j] = low[i];
            j++;
        } else {
            to_output[j] = '\0';
            return to_output;
        }
    }
    return to_output;
}

long binary_to_int(char *binary, int sz) {
    long l = strtol(binary, 0, 2);
    return l;
}

char *underflow_check(char *low, char *high, int size) {
    char *to_output = (char *) malloc(size);
    int j = 0;

    for (int i = 0; i < size; i++) {
        if (low[i] != high[i] && (low[i]=='1' && high[i]=='0')/* || (low[i]=='0' && high[i]=='1')*/) {
        //if(i==0){
            to_output[j] = low[i];
            j++;
        } else {
            to_output[j] = '\0';
            return to_output;
        }
        /*if (i!=0) {
            to_output[i] = low[i];
            j++;
        }*/
    }
    //to_output[j] = '\0';
    return to_output;
}