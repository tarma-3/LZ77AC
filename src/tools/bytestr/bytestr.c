//
// Created by Enrico on 28.11.17.
//
#include <stdlib.h>
#include "bytestr.h"


typedef struct _bytestring {
    unsigned char *buffer;
    size_t capacity;
    size_t len;
} ByteString;

void _outofboundexception() {
    fprintf(stderr, "Error: ByteString out of bounds exception");
    exit(0);
}

ByteString *ba_init(size_t capacity) {
    ByteString *b = malloc(sizeof(ByteString));
    b->buffer = malloc(sizeof(unsigned char) * capacity);
    b->capacity = capacity;
    b->len = 0;
    return b;
}

void bs_set(ByteString *by, size_t position, byte newbyte) {
    if (position < by->capacity) {
        by->buffer[position] = newbyte;
        if (position + 1 > by->len) by->len = position + 1;
        return;
    }
    _outofboundexception();
}

byte bs_get(ByteString *by, size_t position) {
    if (position < by->capacity) {
        return by->buffer[position];
    }
    _outofboundexception();
    return NULL;
}

size_t bs_capacity(ByteString *by) {
    return by->capacity;
}

void bs_setlen(ByteString *by, size_t len) {
    if (len < by->capacity) {
        by->len = len;
        return;
    }
    _outofboundexception();
}

size_t bs_getlen(ByteString *by) {
    return by->len;
}

char *bs_string(ByteString *by, char *buf) {
    int i = 0;
    for (; i < by->len; ++i) {
        buf[i] = by->buffer[i];
    }
    buf[i] = '\0';
    return buf;
}