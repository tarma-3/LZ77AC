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

static const int CAPACITY = 2047;
static unsigned char _buffer[CAPACITY];
static int _elements = 0;
static int _fi_index = 0;
static int _cu_index = 0;

static STATE _status = BEGIN;

void uccb_push(unsigned char newitem) {
    // Nel caso il buffer non sia pieno e quindi non sovrascriviamo nessun carattere
    if (_elements < CAPACITY) {
        _buffer[_elements] = newitem;
        _elements++;
        return;
    }

    // Nel caso il buffer sia pieno e quindi sovrascriviamo il FI
    _buffer[_fi_index] = newitem;

    // Se la posizione del cursore è dove sta il FI allora incrementiamo anche il cursore.
    if (_cu_index == _fi_index) {
        _cu_index = (_fi_index + 1) % CAPACITY;
        _status = BEGIN;
    }
    // Aggiorniamo FI
    _fi_index = ++_fi_index % CAPACITY;
}

bool uccb_hasnext() {
    //begin = clock();
    if (_status == BEGIN) {
        if (_elements == 0) {
            _status = NEXT_NOT_GUARANTEED;
            //cb_hasnext_time += (double) (clock() - begin) / CLOCKS_PER_SEC;
            return false;
        }
        // cb_hasnext_time += (double) (clock() - begin) / CLOCKS_PER_SEC;
        return true;
    }
    // Se il prossimo elemento che leggo è il primo ad essere inserito (e quindi il primo a venir letto) allora devo
    // riportare di non aver un prossimo elemento da leggere poichè sono arrivato all'ultimo.
    if ((_cu_index + 1) % _elements == _fi_index) {
        _status = NEXT_NOT_GUARANTEED;
        //cb_hasnext_time += (double) (clock() - begin) / CLOCKS_PER_SEC;
        return false;
    }
    _status = HAS_NEXT;
    //cb_hasnext_time += (double) (clock() - begin) / CLOCKS_PER_SEC;
    return true;
}

size_t uccb_getid() {
    //Calcolo quanto è distante l'elemento puntato dal primo elemento inserito
    long distance = _fi_index - _cu_index;
    if (distance <= 0)distance += _elements;
    return (size_t) (_elements - distance);
}

unsigned char uccb_next() {
    /*if (cb->status == NEXT_NOT_GUARANTEED) {
        fprintf(stderr, "Error: FIFO -> End of buffer\n");
        exit(1);
    }*/
    if (_status != BEGIN) _cu_index = ++_cu_index % _elements;
    _status = NEXT_NOT_GUARANTEED;
    return _buffer[_cu_index];
}

unsigned char uccb_pointed() {
    if (_status == NEXT_NOT_GUARANTEED) {
        fprintf(stderr, "Error: FIFO -> End of buffer\n");
        exit(1);
    }
    if (_status != BEGIN) return _buffer[(_cu_index + 1) % _elements];
    else
        return _buffer[_cu_index];
}

size_t uccb_nofchars() {
    return _elements;
}

void uccb_setid(size_t id) {
    if (id >= CAPACITY) {
        fprintf(stderr, "Error: Circular Buffer -> Out of bounds\n - cb_setid(%zu)\n", id);
        exit(22);
    }
    _status = NEXT_NOT_GUARANTEED;
    if (id == 0) {
        return uccb_reset();
    }
    _cu_index = (_fi_index + id - 1) % _elements;
}

unsigned char uccb_read(size_t id) {
    if (id >= CAPACITY) {
        fprintf(stderr, "Error: Circular Buffer -> Out of bounds\n - cb_read(%zu)\n", id);
        exit(22);
    }
    uccb_setid(id);
    _cu_index = (_fi_index + id) % _elements;
    return _buffer[_cu_index];
}

void uccb_reset() {
    _cu_index = _fi_index;
    _status = BEGIN;
}

