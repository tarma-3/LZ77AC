//
// Created by alice on 25.10.17.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "element.h"


typedef struct element {
    unsigned char c;
    double start;
    double end;
    double frequency;
    double range;
} Element;

Element *init_element(unsigned char c, double fr) {
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
    printf("Frequency: %lf\n", el->frequency);
    printf("Range: %lf\n", el->range);
    printf("Start: %lf\n", el->start);
    printf("End: %lf\n\n", el->end);
}

void print_element_file(Element *el, FILE *f) {
    fprintf(f,"Char: %c\n", el->c);
    fprintf(f,"Frequency: %lf\n", el->frequency);
    fprintf(f,"Range: %lf\n", el->range);
    fprintf(f,"Start: %lf\n", el->start);
    fprintf(f,"End: %lf\n\n", el->end);
}

double get_element_frequency(Element *el) {
    return el->frequency;
}

double get_element_start(Element *el) {
    return el->start;
}

double get_element_end(Element *el) {
    return el->end;
}

double get_element_range(Element *el) {
    return el->range;
}

void set_element_range(Element *el, double range) {
    el->range = range;
}

void set_element_points(Element *el, double start, double end) {
    el->start = start;
    el->end = end;
}

void set_element_frequency(Element *el, double frq){
    el->frequency=frq;
}