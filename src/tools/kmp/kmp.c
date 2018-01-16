//
// Created by Enrico on 31.12.17.
//

#include "kmp.h"
//
// Created by Enrico on 04/11/2017.
//

#include "../charcb/cb/cibuff.h"
#include "../charcb/charcb.h"
#include "../../lz77/lz77.h"
#include "kmplog.h"
#include <stdlib.h>
#include <time.h>


static const size_t MAX_MATCH = 16;
static PrefixTable prefixtable;

static bool isinit = false;
static ByteString *pattern;
static size_t pattern_i = 0;
const int NMF = -1;

double kmp_match_time = 0;
double kmp_buildtable_time = 0;


typedef struct _prefix_table {
    size_t length;
    int T[MAX_MATCH];
} PrefixTable;

PrefixTable _new_prefixArray(ByteString *P);

void _kmp_updateTable();

void kmp_init() {
    if (isinit == true) {
        fprintf(stderr, "Error on kmp_init: already initialized");
        exit(0);
    }
    pattern = bs_init(MAX_MATCH);
    isinit = true;
}

/**
 * Questa funzione aggiunge un nuovo byte al pattern
 * @param _newbyte
 */
void kmp_addc(unsigned char _newbyte) {
    if (isinit) {
        if (MAX_MATCH == bs_getlen(pattern)) {
            fprintf(stderr, "KMP kmp_addc error: MAX_MATCH reached %ld.\n", bs_getlen(pattern));
            exit(0);
        }
        bs_add(pattern, _newbyte);
        _kmp_updateTable();
    } else {
        fprintf(stderr, "Error on kmp_addc error: kmp not initialized\n");
        exit(0);
    }
}

static int i = 1;
static int j = 0;

//TODO: Ricostruisce la tabella da 0. È possibile riprendere dall'ultimo carattere?
void _kmp_updateTable() {
   // clock_t begin = clock();

    prefixtable.length = bs_getlen(pattern);
    prefixtable.T[0] = 0;
    while (i < prefixtable.length) {
        if (bs_get(pattern, (size_t) i) == bs_get(pattern, (size_t) j)) {
            prefixtable.T[i] = j + 1;
            i++;
            j++;
        } else {
            if (j != 0) {
                j = prefixtable.T[j - 1];
            } else {
                prefixtable.T[i] = 0;
                i++;
            }
        }
    }

    //clock_t end = clock();
   // kmp_buildtable_time += (double) (end - begin) / CLOCKS_PER_SEC;
#if DEBUG_KMP_LOG
    if (DEBUG_ENABLED) __log_prefixtable(pattern, prefixtable.T, prefixtable.length);
#endif
}


long kmp_match(CircularBuffer *cb) {
    //clock_t begin = clock();
    while (uccb_hasnext(cb)) {
        //hasn_time_end = clock();
       // hasnext_time += (double) (hasn_time_end - hasn_time_begin) / CLOCKS_PER_SEC;

        if (uccb_pointed(cb) == bs_get(pattern, pattern_i)) {
            uccb_next(cb);
            ++pattern_i;
        } else {
            if (pattern_i != 0) {
                pattern_i = (size_t) prefixtable.T[--pattern_i];
            } else {
                uccb_next(cb);
            }
        }
        if (pattern_i == prefixtable.length) {
            //clock_t end = clock();

            //kmp_match_time += (double) (end - begin) / CLOCKS_PER_SEC;
            return (long) uccb_nofchars(cb) - uccb_getid(cb);
        }
    }

    //clock_t end = clock();
    //kmp_match_time += (double) (end - begin) / CLOCKS_PER_SEC;
    return NMF;
}

size_t kmp_patternlen() {
    return bs_getlen(pattern);
}

char kmp_getlastc() {
    if (bs_getlen(pattern) == 0) {
        exit(0);
    }
    return bs_get(pattern, bs_getlen(pattern) - 1);
}

/** Questa funzione resetta il pattern. Non resetta il circular buffer
 */
void kmp_reset() {
    pattern_i = 0;
    bs_setlen(pattern, 0);
    i = 1;
    j = 0;
}

bool kmp_isfull() {
    if (MAX_MATCH == bs_getlen(pattern)) {
        return true;
    } else return false;
}