//
// Created by Enrico on 05.11.17.
//

#include <printf.h>
#include "kmpsearch.h"
int main() {
    char P[] = "ABXABC";
    char S[] = "ABXABXABC";
    printf("%ld", strnmatch(P, S,1));
    return 0;
}