//
// Created by Enrico on 31.12.17.
//

#include "kmplog.h"
#include "../../lz77/lz77.h"

FILE *log_kmp;
void __log_prefixtable(ByteString *P, int *T, size_t _Tsize) {
    log_kmp = fopen("./log/log_kmp.txt", "a");
    fprintf(log_kmp, "| Pattern to match:  ");
    for (unsigned int i = 0; i < _Tsize; i++) {
        fprintf(log_kmp, "[%c / %3d]", bs_get(P, i), bs_get(P, i));
    }
    fprintf(log_kmp, "\n");
    fprintf(log_kmp, "| T-Array:          [");
    for (unsigned int i = 0; i < _Tsize; i++) {
        fprintf(log_kmp, "[%7d]", T[i]);
    }
    fprintf(log_kmp, "]\n");
    fclose(log_kmp);
}

void __log_kmp_match(long result){
    if (DEBUG_ENABLED) {
        log_kmp = fopen("./log/log_kmp.txt", "a");
        fprintf(log_kmp, "[RETURN] %ld\n\n", result);
        fclose(log_kmp);
    }
}