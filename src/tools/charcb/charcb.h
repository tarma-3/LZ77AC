//
// Created by Enrico on 12.11.17.
//

#ifndef LZ77AC_CHARCB_H
#define LZ77AC_CHARCB_H


#include <stddef.h>
#include <stdbool.h>
#include "cb/cibuff.h"


// "Constructors & destructors"
CircularBuffer *uccb_init(size_t capacity);
void uccb_free(CircularBuffer *cb);


// Setters
bool uccb_push(unsigned char uchar,CircularBuffer *cb);
size_t uccb_getid(CircularBuffer *cb);
void uccb_reset(CircularBuffer *cb);

unsigned char uccb_read(size_t id, CircularBuffer *cb);
unsigned char uccb_next(CircularBuffer *cb);
unsigned char uccb_pointed(CircularBuffer *cb);

bool uccb_hasnext(CircularBuffer *cb);
size_t uccb_nofchars(CircularBuffer *cb);

//int uccb_errorcode(CircularBuffer *cb);

#endif //LZ77AC_CHARCB_H
