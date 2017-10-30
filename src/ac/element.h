//
// Created by alice on 25.10.17.
//
#include <stdint.h>

#ifndef LZ77AC_ELEMENT_H
#define LZ77AC_ELEMENT_H
typedef struct element Element;

Element *init_element(unsigned char c, double fr);

void free_element(Element *el);

double get_element_start(Element *el);

double get_element_end(Element *el);

unsigned char get_element_char(Element *el);

void print_element(Element *el);

void print_element_file(Element *el, FILE *f);

double get_element_frequency(Element *el);

void set_element_range(Element *el, double range);

void set_element_points(Element *el, double start, double end);

void set_element_frequency(Element *el, double frq);

double get_element_range(Element *el);

#endif //LZ77AC_ELEMENT_H
