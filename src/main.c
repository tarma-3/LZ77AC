#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "ac/ac_encoding.h"
#include "ac/ac_decoding.h"

int stream_file_to(char *args, void (*output_handler)(unsigned char)) {
    // This function return 1 if the file exists
    FILE *input;
    int next_char;
    if ((input = fopen(args, "r")) != 0 && ((next_char = fgetc(input)) >= 0)) {
        while (next_char != EOF) {
            output_handler((unsigned char) next_char);
            next_char = fgetc(input);
        }
        fclose(input);
        return 1;
    }
    return 0;
}

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

        //Build frequencies
        stream_file_to(args[2], build_frequency);
        //init files and write total_char, frq
        init_co();
        //Stream the frequency array to
        //build ranges first time
        stream_array_to(get_frequency(), ac_ranges);
        //stream file to encode to
        //ac_encode
        stream_file_to(args[2], ac_encode);

        t = clock() - t;
        double time_taken = ((double)t)/CLOCKS_PER_SEC;
        printf("Exec compression: %lf\n",time_taken);

    } else if (strcmp("-d", args[1]) == 0) {
        // DECOMPRESSION
        // lz77_decode(ac_decode(args[1]));
        clock_t t = clock();

        init_wa(args[2]);
        ac_decode();

        t = clock() - t;
        double time_de = ((double)t)/CLOCKS_PER_SEC;
        printf("Exec DEcompression: %lf\n",time_de);
    } else {
        fprintf(stderr, "Error on first parameter '%s': Invalid option", args[1]);
        exit(107);
    }

    return 0;
}