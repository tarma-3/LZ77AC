//
// Created by Enrico on 31.12.17.
//

#ifndef LZ77AC_KMPLOG_H
#define LZ77AC_KMPLOG_H
#include "../bytestr/bytestr.h"
void __log_prefixtable(ByteString *P, int *T, size_t _Tsize);
void __log_kmp_match(long result);
#endif //LZ77AC_KMPLOG_H
