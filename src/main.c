#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./lz77/lz77.h"


int main(int args_number, char *args[]) {
    if(args_number!=3){
        printf("usage: a.out option source_file\n");
        exit(2);
    }

    // Option check
    if(strcmp("-c",args[1])==0){
        lz77_encode(args[2]);
        // COMPRESSION
        // ac_encode(lz77_encode(args[1]));
    }
    else if(strcmp("-d",args[1])==0){
        lz77_decode(args[2]);


        // DECOMPRESSION
        // lz77_decode(ac_decode(args[1]));
    }
    else {
        fprintf(stderr,"Error on first parameter '%s': Invalid option\n", args[1]);
        exit(107);
    }

    return 0;
}