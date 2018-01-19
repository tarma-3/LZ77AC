//
// Created by Enrico on 17.12.17.
//

#include <stdio.h>
#include "bitfilewriter.h"

int main(int args_number, char *args[]){
    unsigned char c = 'a';
    BitfileWriter *fp = newBitFileWriter("./log/testbitfile.txt", BF_WRITE);
    for (int i = 1; i<=8; ++i) {
        write_bit(c>>8-i,fp);
    }
    c = 'f';
    for (int i = 1; i<=8; ++i) {
        write_bit(c>>8-i,fp);
    }
    c = '\n';
    for (int i = 1; i<=8; ++i) {
        write_bit(c>>8-i,fp);
    }
    c = 'k';
    for (int i = 1; i<=8; ++i) {
        write_bit(c>>8-i,fp);
    }
    c = 'm';
    write_bits(8,c,fp);

    c = 0x6;
    write_bits(4,c,fp);

    c = 0x16;
    write_bits(8,c,fp);

    c = 0x1;
    write_bits(4,c,fp);
    bfclose(fp);
    fp = newBitFileWriter("./log/testbitfile.txt", BF_READ);
    fprintf(stdout,"%d",read_bits(3,fp));
    fprintf(stdout,"%d",read_bits(1,fp));
    fprintf(stdout,"%d ",read_bits(4,fp));
    fprintf(stdout,"%d",read_bits(4,fp));
    fprintf(stdout,"%d ",read_bits(4,fp));
    fprintf(stdout,"%d",read_bits(4,fp));
    fprintf(stdout,"%d ",read_bits(4,fp));

}