//
// Created by Enrico on 08.11.17.
//

#include <printf.h>
#include "../charcb.h"

int main() {
    CircularBuffer *cb = cb_init(30, sizeof(char));
    char c;
    char S0[] = "Copyright (C) 2007 Free Software Foundation\"\n"
            "Everyone is permitted to copy and distribute verbatim copies of this license document,";
    size_t sLen = strlen(S0);

    for (int i = 0; i < sLen; ++i) {
        cb_push(&S0[i],cb);
    }


    while (cb_hasnext(cb)){
        cb_next(&c, cb);
        printf("%c",c);
    }

    return 0;
}