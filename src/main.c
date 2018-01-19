#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "ac/ac_encoding.h"
#include "ac/ac_decoding.h"
#include "lz77/lz77.h"
#include "tools/file/file.h"


int stream_array_to(int frequency[], void (*output_handler)(unsigned char, int)) {
    //Send each char with relative frequency from array frequency
    //only if frequency is not empty
    //to handler function
    for (int i = 0; i < 255; i++) {
        if (frequency[i] != 0) {
            output_handler((unsigned char) frequency[i], i);
        }
    }
    return 1;
}

int main(int args_number, char *args[]) {
    if (args_number != 3) {
        printf("usage: a.out option source_file");
        exit(2);
    }

    // Option check
    if (strcmp("-c", args[1]) == 0) {
        clock_t t;
        t = clock();

        size_t extlen = get_extlen(args[2]);
        char outputname[strlen(args[2])+5];
        strcpy(outputname,args[2]);
        strcpy(&outputname[strlen(args[2])-extlen],".press");

        lz77_encode(args[2]);
        stream_file_to("tmp.press", build_frequency);
        init_co(outputname);
        stream_array_to(get_frequency(), ac_ranges);
        stream_file_to("tmp.press", ac_encode);

        remove("tmp.press");

        t = clock() - t;
        double time_taken = ((double)t)/CLOCKS_PER_SEC;
        printf("Exec compression: %lf\n",time_taken);

    } else if (strcmp("-d", args[1]) == 0) {
        clock_t t = clock();

        init_wa(args[2]);
        ac_decode();
        lz77_decode("tmp",args[2]);
        remove("tmp");
        t = clock() - t;
        double time_de = ((double)t)/CLOCKS_PER_SEC;
        printf("Exec DEcompression: %lf\n",time_de);
    } else {
        fprintf(stderr, "Error on first parameter '%s': Invalid option", args[1]);
        exit(107);
    }

    return 0;
}