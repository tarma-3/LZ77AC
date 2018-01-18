//
// Created by alice on 08.11.17.
//

#include <stdint.h>

#ifndef LZ77AC_AC_DECODING_H
#define LZ77AC_AC_DECODING_H

void dac_ranges(int i);

void ac_decode();

void set_frequency(int *frq, int len);

void set_total_char(int t_ch);

void read_in_32();

void set_to_counter_EOF();

void init_wa(char *ac_output);

#endif //LZ77AC_AC_DECODING_H
