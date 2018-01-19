//
// Created by Enrico on 12.11.17.
//

#ifndef LZ77AC_CHARCB_H
#define LZ77AC_CHARCB_H


#include <stddef.h>
#include <stdbool.h>


// "Constructors & destructors"
void uccb_free();

// Setters
void uccb_push(unsigned char uchar);
size_t uccb_getid();
void uccb_reset();

unsigned char uccb_next();
unsigned char uccb_pointed();
bool uccb_hasnext();
size_t uccb_nofchars();
void uccb_setid(size_t id);


#endif //LZ77AC_CHARCB_H
