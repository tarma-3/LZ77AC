//
// Created by Enrico on 09/10/2017.
//

#include "lz77.h"
#include "../tools/circular_buffer.h"
#include "../tools/fifo.h"
#include "../tools/file.h"

#define DEFAULT_OFFSET 2048

//Numero caratteri massimi nel dizionario
int offset = DEFAULT_OFFSET;
struct circular_buffer *cb;
// 1123451231231231

void init_encode(){
    cb = fifo_init(10,sizeof(unsigned char));
}

void run_encode(unsigned char a){
    int matches = 0;
    unsigned char i;
    while (fifo_next(&i,cb)){
        if(i==a){
            printf("%c -> b\n",a);
            matches++;
        }
    }
    if(!matches){
        printf("%c -> a\n",a);
    }
    fifo_push(&a, cb);
}

char *lz77_encode(char args[]){
    init_encode();
    stream_file_to(args,run_encode);
}