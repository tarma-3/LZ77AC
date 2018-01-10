//
// Created by Enrico on 08.11.17.
//

#include <printf.h>
#include "cibuffo.h"
#include "../charcb.h"

int main() {
    CircularBuffer *cb = cb_init(10, sizeof(char));
    char c;
    char S0[] = "Copyr";
    char S[] = "ight (C) 2007 Free Software Foundation"
            " Everyone is permitted to copy and distribute verbatim copies"
            " of this license document, but changing it is not allo";
    char S2[] = "wed.";
    size_t sLen = strlen(S0);
    for (int i = 0; i < sLen; ++i) {
        cb_push(&S0[i],cb);
    }
    cb_read(2,&c,cb);

    sLen = strlen(S);
    for (int i = 0; i < sLen; ++i) {
        cb_push(&S[i],cb);
    }


    for(size_t i = 5;i<7;i++){
        cb_read(i,&c,cb);
        printf("%c",c);
    }
    printf("#");
    sLen = strlen(S2);
    for (int i = 0; i < sLen; ++i) {
        cb_push(&S2[i],cb);
    }

    while (cb_hasnext(cb)){
        cb_next(&c, cb);
        printf("%c",c);
    }

    return 0;
}