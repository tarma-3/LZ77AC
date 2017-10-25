//
// Created by alice on 25.10.17.
//

#include <stdio.h>
#include <stdlib.h>
#include "element.h"


typedef struct element {
    unsigned char c;
    unsigned long long int start;
    unsigned long long int end;
    int frequency;
    int range;
} Element;

Element *initElement(unsigned char c, int fr) {
    Element *el;
    el = malloc(sizeof(Element));
    el->c = c;
    el->frequency = fr;
    return el;
}

void freeElement(Element *el) {
    free(el);
}
void printElement(Element *el){
    printf("Char: %c\n", el->c);
    printf("Frequency: %d\n", el->frequency);
    printf("Range: %d\n", el->range);
    printf("Start: %llu\n", el->start);
    printf("End: %llu\n\n", el->end);
}