//
// Created by alice on 10.10.17.
//

#ifndef LZ77AC_AC_ENCODING_H
#define LZ77AC_AC_ENCODING_H

void ac_ranges(unsigned char next_char, int i);

void ac_encode(unsigned char next_char);

void build_frequency(unsigned char next_char);

void init_co();

int *get_frequency();

void ac_end();


#endif //LZ77AC_AC_ENCODING_H