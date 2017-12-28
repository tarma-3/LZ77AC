//
// Created by Enrico on 28.11.17.
//

#ifndef LZ77AC_BYTEARRAY_H
#define LZ77AC_BYTEARRAY_H

#include <stdio.h>

typedef unsigned char byte;
typedef struct _bytestring ByteString;

ByteString *ba_init(size_t capacity);

void bs_set(ByteString *by, size_t position, byte newbyte);

byte bs_get(ByteString *by, size_t position);

size_t bs_getlen(ByteString *by);
void bs_setlen(ByteString *by,size_t len);

size_t bs_capacity(ByteString *by);

char *bs_string(ByteString *by, char *buf);

#endif //LZ77AC_BYTEARRAY_H
