//
// Created by Enrico on 04/11/2017.
//

#include <strings.h>
#include "../charcb/cb/cibuff.h"
#include "../bytestr/bytestr.h"

long strnmatch(const char *P, const char *S, size_t n);
long strmatch(const char *P, const char *S);
long upmatch(ByteString *P, CircularBuffer *cb, size_t from);