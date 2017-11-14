//
// Created by Enrico on 04/11/2017.
//

#include <strings.h>
#include "../charcb/cb/cibuff.h"

long strnmatch(const char *P, const char *S, size_t n);
long strmatch(const char *P, const char *S);
long structmatchn(char *P,CircularBuffer *cb,size_t n);