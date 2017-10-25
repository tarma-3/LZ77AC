//
// Created by alice on 25.10.17.
//

#ifndef LZ77AC_ELEMENT_H
#define LZ77AC_ELEMENT_H
typedef struct element Element;

Element *initElement(unsigned char c, int fr);

void freeElement(Element *el);

void printElement(Element *el);

#endif //LZ77AC_ELEMENT_H
