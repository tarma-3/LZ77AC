//
// Created by alice on 08.11.17.
//

#include <stdint.h>

#ifndef LZ77AC_AC_DECODING_H
#define LZ77AC_AC_DECODING_H

void dac_ranges(unsigned char next_char, int i);

void ac_decode();

void set_frequency(int *frq, int len);

void set_total_char(int t_ch);

void read_in_32();

#endif //LZ77AC_AC_DECODING_H
