//
// Created by Enrico on 12.11.17.
//

#include <stdlib.h>
#include <stdio.h>
#include "scharcb.h"

// "Constructors & destructors"
typedef enum _state {
    BEGIN, HAS_NEXT, NEXT_NOT_GUARANTEED
} STATE;

#define CAPACITY 2047
static unsigned char _buffer[CAPACITY];
static int _elements = 0;
static int _fi_index = 0;
static int _bf_index = 0;



void uccb_push(unsigned char newitem) {
    // Nel caso il buffer non sia pieno e quindi non sovrascriviamo nessun carattere
    if (_elements < CAPACITY) {
        _buffer[_elements] = newitem;
        _elements++;
        return;
    }
    if (_bf_index != 0)_bf_index--;
    // Aggiorniamo FI
    _fi_index = ++_fi_index % CAPACITY;
}

bool uccb_hasnext() {
    if (_elements == 0 || _bf_index == _elements) {
        return false;
    }

    return true;
}

size_t uccb_getid() {
    return _bf_index - 1;
}

unsigned char uccb_next() {
    return _buffer[(_fi_index + _bf_index++) % _elements];
}

unsigned char uccb_pointed() {
    return _buffer[(_fi_index + _bf_index) % _elements];
}

size_t uccb_nofchars() {
    return _elements;
}

void uccb_setid(size_t id) {
    _bf_index = id;
}

void uccb_reset() {
    _bf_index = 0;
}

