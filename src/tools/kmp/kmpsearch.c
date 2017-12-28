//
// Created by Enrico on 04/11/2017.
//

#include "kmpsearch.h"
#include "../charcb/cb/cibuff.h"
#include "../charcb/charcb.h"
#include <stdlib.h>
#include <time.h>

typedef struct _prefix_arr {
    size_t length;
    int *T;
} PrefixArray;

#if DEBUG_KMP_LOG
FILE *log_kmp;

void __log_prefixtable(ByteString *P, PrefixArray *T) {
    log_kmp = fopen("./log/log_kmp.txt", "a");
    fprintf(log_kmp, "| Pattern to match:  ");
    for (unsigned int i = 0; i < T->length; i++) {
        fprintf(log_kmp, "[%c / %3d]", bs_get(P, i), bs_get(P, i));
    }
    fprintf(log_kmp, "\n");
    fprintf(log_kmp, "| T-Array:          [");
    for (unsigned int i = 0; i < T->length; i++) {
        fprintf(log_kmp, "[%7d]", T->T[i]);
    }
    fprintf(log_kmp, "]\n");
    fclose(log_kmp);
}

#endif

PrefixArray *_ubuild_table(ByteString *P) {
    PrefixArray *parr;
    parr = malloc(sizeof(struct _prefix_arr));
    parr->length = bs_getlen(P);
    parr->T = malloc(sizeof(int) * parr->length);
    parr->T[0] = 0;
    int i = 1;
    int j = 0;
    while (i < parr->length) {
        if (bs_get(P, (size_t) i) == bs_get(P, (size_t) j)) {
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
#if DEBUG_KMP_LOG
    __log_prefixtable(P, parr);
#endif
    return parr;
}

PrefixArray *_build_table(char P[]) {
    return _ubuild_table(P);
}

void _free_table(PrefixArray *parr) {
    free(parr->T);
    free(parr);
}

long _core(const char P[], const char S[], PrefixArray *parr, size_t n) {
    size_t ssize = strlen(S);
    for (size_t l = 0; n < ssize;) {
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

long _core_struct(ByteString *_P_lookahe_buff, CircularBuffer *cb, PrefixArray *_T_prefixarray, size_t pattern_index) {
    pattern_index = (size_t) _T_prefixarray->T[pattern_index];
    while (uccb_hasnext(cb)) {
        if (uccb_pointed(cb) == bs_get(_P_lookahe_buff, pattern_index)) {
            uccb_next(cb);
            ++pattern_index;
        } else {
            if (pattern_index != 0) {
                pattern_index = (size_t) _T_prefixarray->T[--pattern_index];
            } else uccb_next(cb);
        }
        // Raggiunta la fine del pattern, match trovato
        if (pattern_index == _T_prefixarray->length) {
            if (uccb_getid(cb) == 0)
                return (long) (uccb_nofchars(cb)) - _T_prefixarray->length;
            return (long) uccb_getid(cb) - _T_prefixarray->length;
        }
    }
    return -1;
}

int CLEAR_LOG = 1;

long strnmatch(const char *P, const char *S, size_t n) {
#if DEBUG_KMP_LOG
    if (CLEAR_LOG) {
        log_kmp = fopen("./log/log_kmp.txt", "wb+");
        fclose(log_kmp);
    }
    CLEAR_LOG = 0;
#endif
    PrefixArray *T = _build_table(P);
    long result = _core(P, S, T, n);
    _free_table(T);
    return result;
}

long strmatch(const char *P, const char *S) {
    return strnmatch(P, S, 0);
}

/**
 * Unsigned Pattern Matching
 * Questa funzione permette, dato una stringa da confrontare, di trovare la sua corrispondenza esatta allinterno del
 * CircularBuffer.
 *
 * Da notare che la seguente funzione accetta un array di caratteri @parm P ignorando il terminatore '\0'.
 * Per questo motivo è necessario scrivere quanto deve essere considerato lungo in nostro array di caratteri nel
 * campo @param len
 * @param P La stringa per la quale si vuole trovare una corrispondenza
 * @param cb Il CircularBuffer all'interno il quale si vuole trovare la stringa
 * @param from indica da che punto di P vogliamo continuare la ricerca
 * @param len indica da che punto di P vogliamo continuare la ricerca
 * @return
 */
long upmatch(ByteString *P, CircularBuffer *cb, size_t from) {
#if DEBUG_KMP_LOG
    if (CLEAR_LOG) {
        log_kmp = fopen("./log/log_kmp.txt", "wb+");
        char buffer[26];
        {
            time_t timer;
            time(&timer);
            struct tm *tm_info = localtime(&timer);
            strftime(buffer, 26, "\n %Y-%m-%d [%H:%M]", tm_info);
        }
        fprintf(log_kmp, "%s - INIT FROM pm_ntof()\n\n", buffer);
        fclose(log_kmp);
    }
    CLEAR_LOG = 0;
    log_kmp = fopen("./log/log_kmp.txt", "a");
    char t[bs_capacity(P)];
    fprintf(log_kmp, "[CALLED] pm_ntof(%s,cb,%d)\n", bs_string(P, t), (int) from);
    fclose(log_kmp);
#endif

    PrefixArray *T = _ubuild_table(P);
    long result = _core_struct(P, cb, T, from);
    _free_table(T);

#if DEBUG_KMP_LOG
    log_kmp = fopen("./log/log_kmp.txt", "a");
    fprintf(log_kmp, "[RETURN] %ld\n\n", result);
    fclose(log_kmp);
#endif
    return result;
}

