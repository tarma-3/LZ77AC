//
// Created by Enrico on 05.01.18.
//
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>

typedef struct _circularbuffer CircularBuffer;

extern double cb_hasnext_time;

CircularBuffer *cb_init(size_t capacity, size_t sz);

bool cb_push(const void *newitem, CircularBuffer *cb);

size_t cb_getid(CircularBuffer *cb);

bool cb_hasnext(CircularBuffer *cb);

size_t cb_next(void *item, CircularBuffer *cb);

size_t cb_pointed(void *item, CircularBuffer *cb);

size_t cb_nofel(CircularBuffer *cb);

void cb_read(size_t id, void *item, CircularBuffer *cb);

void cb_setid(size_t id, CircularBuffer *cb);

void cb_reset(CircularBuffer *cb);

void cb_free(CircularBuffer *cb);

#ifndef LZ77AC_CIBUFF_H
#define LZ77AC_CIBUFF_H


#endif //LZ77AC_CIBUFF_H
