//
// Created by Enrico on 04/11/2017.
//

#include "kmpsearch.h"
#include "../charcb/cb/cibuff.h"
#include "../charcb/charcb.h"
#include <stdlib.h>

typedef struct _prefix_arr {
    size_t length;
    int *T;
} PrefixArray;

typedef struct _kmpsearch{
    PrefixArray pr_arr;
    bool is_init;
} KMPSearch;

PrefixArray *_build_table(char P[]) {
    PrefixArray *parr;
    parr = malloc(sizeof(struct _prefix_arr));

    parr->length = strlen(P);
    parr->T = malloc(sizeof(int) * parr->length);
    parr->T[0] = 0;

    int i = 1;
    int j = 0;
    while (i < parr->length) {
        if (P[i] == P[j]) {
            parr->T[i] = j + 1;
            i++;
            j++;
        } else {
            if (j != 0) {
                j = parr->T[j - 1];
            } else {
                parr->T[i] = 0;
                i++;
            }
        }
    }
    return parr;
}

void _free_table(PrefixArray *parr) {
    free(parr->T);
    free(parr);
}

long _core(const char P[], const char S[], PrefixArray *parr, size_t n) {
    size_t ssize = strlen(S);
    for (size_t l = 0; n<ssize;) {
        if (S[n] == P[l]) {
            n++;
            l++;
        } else {
            if (l != 0) {
                l = (size_t) parr->T[--l];
            } else n++;
        }
        if (l == parr->length) {
            return (long) n - parr->length;
        }
    }
    return -1;
}

long _core_struct(const char *P, CircularBuffer *cb, PrefixArray *parr,size_t pattern_index) {
    while (uccb_hasnext(cb)) {
        if (uccb_pointed(cb) == P[pattern_index]) {
            uccb_next(cb);
            pattern_index++;
        }
        else {
            if (pattern_index != 0) {
                pattern_index = (size_t) parr->T[--pattern_index];
            } else uccb_next(cb);
        }
        // Pattern finito, match trovato
        if (pattern_index == parr->length) {
            return (long) uccb_getid(cb) - parr->length;
        }
    }
    return -1;
}

long strnmatch(const char *P, const char *S,size_t n) {
    PrefixArray *T = _build_table(P);
    long result = _core(P, S, T,n);
    _free_table(T);
    return result;
}

long strmatch(const char *P, const char *S) {
    return strnmatch(P,S,0);
}

long structmatchn(char *P,CircularBuffer *cb,size_t pattern_index){
    PrefixArray *T = _build_table(P);
    long result = _core_struct(P, cb, T,pattern_index);
    _free_table(T);
    return result;
}

