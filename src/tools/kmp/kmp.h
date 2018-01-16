//
// Created by Enrico on 31.12.17.
//

#ifndef LZ77AC_KMP_H
#define LZ77AC_KMP_H

#include "../bytestr/bytestr.h"
#include "../charcb/cb/cibuff.h"

typedef struct _prefix_table PrefixTable;
extern const int NMF;
extern double kmp_match_time;
extern double kmp_buildtable_time;


void kmp_init();
void kmp_reset();

void kmp_addc(unsigned char _newbyte);

long kmp_match(CircularBuffer *cb);

size_t kmp_patternlen();

char kmp_getlastc();

bool kmp_isfull();

bool kmp_samematch();

#endif //LZ77AC_KMP_H
