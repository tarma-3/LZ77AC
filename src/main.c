#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void open_file(char args[]){
    FILE *input;
    input = fopen(args, "r");

    int next_char = fgetc(input);
    while (next_char != EOF) {
        printf("%c",next_char);
        next_char = fgetc(input);
    }
}

int main(int args_number, char *args[]) {
    for (int i = 0; i < args_number; ++i)
    {
        printf("argv[%d]: %s\n", i, args[i]);
    }

    if(strcmp("-c",args[1])==0){
        //COMPRESSION
    }
    else if(strcmp("-d",args[1])==0){
        //DECOMPRESSION
    }
    else {
        printf("Invalid option");
        exit(0);
    }

    open_file(args[2]);

    return 0;
}