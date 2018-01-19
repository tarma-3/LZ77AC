//
// Created by Enrico on 29.12.17.
//

#include <stdio.h>
#include "utility.h"

char *__tobinary(int size, long num) {
    static char rtn[12];
    if (size > 11) {
        fprintf(stderr, "\n Error on __tobinary: input number to big");
    }
    for (int i = 0; i < size; ++i) {
        rtn[i] = (char) ('0' + ((num >> ((size - 1) - i)) & 1));
    }
    rtn[size] = '\0';
    return rtn;
}

char *_tobinary(char *dest, int size, long num) {

    for (int i = 0; i < size; ++i) {
        dest[i] = (char) ('0' + ((num >> ((size - 1) - i)) & 1));
    }
    dest[size] = '\0';
    return dest;
}

char *_tobinarylr(char *dest, int size, char num) {
    unsigned char num1 = (unsigned char) num;
    for (int j = 0; j < size; ++j) {
        dest[j] = (char) ('0' + (num1 >> 7));
        num1 <<= 1;
    }
    dest[size] = '\0';
    return dest;
}