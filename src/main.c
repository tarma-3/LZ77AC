#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_char(unsigned char c){
    printf("%c",c);
}

int main(int args_number, char *args[]) {

    if(args_number!=3){
        printf("usage: a.out option source_file");
        exit(2);
    }

    // Option check
    if(strcmp("-c",args[1])==0){
        // COMPRESSION
        // ac_encode(lz77_encode(args[1]));
    }
    else if(strcmp("-d",args[1])==0){
        // DECOMPRESSION
        // lz77_decode(ac_decode(args[1]));
    }
    else {
        fprintf(stderr,"Error on first parameter '%s': Invalid option", args[1]);
        exit(107);
    }

    return 0;
}