//
// Created by alice on 25.10.17.
//
#include <stdint.h>
#include <stdio.h>

#ifndef LZ77AC_ELEMENT_H
#define LZ77AC_ELEMENT_H
typedef struct element Element;

Element *init_element(unsigned char c, uint32_t fr);

void free_element(Element *el);

uint32_t get_element_start(Element *el);

uint32_t get_element_end(Element *el);

unsigned char get_element_char(Element *el);

void print_element(Element *el);

void print_element_file(Element *el, FILE *f);

int get_element_frequency(Element *el);

void set_element_range(Element *el, uint32_t range);

void set_element_points(Element *el, uint32_t start, uint32_t end);

void set_element_frequency(Element *el, uint32_t frq);

uint32_t get_element_range(Element *el);

int is_in_range(Element *el, uint32_t value);

char *int_to_binary(unsigned int val, int sz);

char *check_output_range(char *low, char *high, int size);

long binary_to_int(char *binary, int sz);

char *underflow_check(char *low, char *high, int size);

#endif //LZ77AC_ELEMENT_H
