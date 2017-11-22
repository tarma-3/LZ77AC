//
// Created by alice on 08.11.17.
//

#include <stdint.h>

#ifndef LZ77AC_AC_DECODING_H
#define LZ77AC_AC_DECODING_H

void dac_ranges(int *frequency, uint32_t output);

void ac_decode(int *frq);

int *set_frequency(int *frq, int len);

void set_total_char(int t_ch);

#endif //LZ77AC_AC_DECODING_H
