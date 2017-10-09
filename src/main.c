#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void pour_file_to(char *args, void (*output_handler)(unsigned char)){
    FILE *input;
    input = fopen(args, "r");

    int next_char = fgetc(input);
    while (next_char != EOF) {
        output_handler((unsigned char)next_char);
        next_char = fgetc(input);
    }
}

void print_char(unsigned char c){
    printf("%c",c);
}

int main(int args_number, char *args[]) {

    if(args_number!=3){
        printf("usage: a.out option source_file");
        exit(2);
    }

    if(strcmp("-c",args[1])==0){
        // COMPRESSION
        // ac_encode(lz77_encode(args[1]));
    }
    else if(strcmp("-d",args[1])==0){
        // DECOMPRESSION
        // lz77_decode(ac_decode(args[1]));
    }
    else {
        printf("Error: Invalid option '%s'", args[1]);
        exit(2);
    }
    // Check if file exists
    if( access(args[2], F_OK ) == -1 ) {
        // file doesn't exist
        printf("Error: File not found");
        exit(2);
    }

    pour_file_to(args[2], print_char);

    return 0;
}