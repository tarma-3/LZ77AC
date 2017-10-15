//
// Created by Enrico on 09/10/2017.
//

#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include <stdlib.h>
#include "file.h"
int stream_file_to(char *args,void (*output_handler)(unsigned char)){
    // This function return 1 if the file exists
    FILE *input;
    int next_char;
    if((input = fopen(args, "r"))!=0&&((next_char = fgetc(input))>=0)) {
        while (next_char != EOF) {
            output_handler((unsigned char) next_char);
            next_char = fgetc(input);
        }
        fclose(input);
        return 1;
    }
    fprintf(stderr,"Error on 2nd parameter '%s': %s\n",args,strerror(errno));
    return 0;
}

void print_char(unsigned char c){
    printf("%c",c);
}

// USAGE EXAMPLE
/*int main(int args_number, char *args[]){

    if(!stream_file_to(args[1],print_char)){
        // If file not exists
        exit(errno);
    }
}*/