#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
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

void print_char(unsigned char c) {
    printf("%c", c);
}

int main(int args_number, char *args[]) {

    if (args_number != 3) {
        printf("usage: a.out option source_file");
        exit(2);
    }

    // Option check
    if (strcmp("-c", args[1]) == 0) {
        //Build frequencies
        stream_file_to(args[2], build_frequency);
        //Print file Log debug
        print_frequency();
        //Stream the frequency array to
        //build ranges first time
        stream_array_to(get_frequency(), ac_ranges);

        //stream file to encode to
        //ac_encode
        stream_file_to(args[2], ac_encode);

        // COMPRESSION
        // ac_encode(lz77_encode(args[1]));


        //TEST DECOMPRESSION
        // ** IN PROGRESS, NOT WORKING **
        set_total_char(get_total_char());
        set_frequency(get_frequency(), 255);
        read_in_32();
        stream_array_to(get_frequency(), dac_ranges);
        ac_decode();
        //read_output();

    } else if (strcmp("-d", args[1]) == 0) {
        // DECOMPRESSION
        // lz77_decode(ac_decode(args[1]));
    } else {
        fprintf(stderr, "Error on first parameter '%s': Invalid option", args[1]);
        exit(107);
    }
    // Check if file exists
    if (!stream_file_to(args[2], print_char)) {
        // If file not exists
        fprintf(stderr, "Error on 2nd parameter '%s': %s\n", args[2], strerror(errno));
        exit(errno);
    }

    return 0;
}