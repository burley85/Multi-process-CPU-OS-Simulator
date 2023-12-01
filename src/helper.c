#include <stdbool.h>
#include <stdio.h>

void print_bin(unsigned char* buffer, int length){
    for(int i = 0; i < length; i++){
        for(int j = 0; j < 8; j++){
            printf("%d", (buffer[i] & (0b10000000 >> j)) != 0);
        }
        printf(" ");
    }
    printf("\n");
}
void cpy_bit(unsigned char* dest, unsigned char dest_offset, unsigned char* src, unsigned char src_offset){
    if(dest_offset > 7 || dest_offset < 0){
        printf("ERROR: Cannot copy to %d-th bit\n", dest_offset);
        return;
    }
    if(src_offset > 7 || src_offset < 0){
        printf("ERROR: Cannot copy from %d-th bit\n", src_offset);
        return;
    }
    //Get scr_bit
    unsigned char src_mask = 0b10000000 >> src_offset;
    bool src_bit = ((*src & src_mask) != 0);

    //Get dest_bit
    unsigned char dest_mask = 0b10000000 >> dest_offset;
    bool dest_bit = ((*dest & dest_mask) != 0);

    if(src_bit == dest_bit) return;
    //Set dest_bit
    if(src_bit) *dest = *dest + dest_mask;
    //Clear dest_bit
    else *dest = *dest - dest_mask;
}

//Memcpy that can start in the middle of a byte
//dest is the address of the first byte to copy to
//dest_offset is the bit number to start copying to
//src is the address of the first byte to copy from
//src_offset is the bit number to start copying from
//num_bits is the number of bits to copy
void memcpy_bits(void* destVoid, unsigned char dest_offset, void* srcVoid, unsigned char src_offset, int num_bits){
    unsigned char* dest = destVoid;
    unsigned char* src = srcVoid;

    if(dest_offset > 7 || dest_offset < 0){
        printf("ERROR: Cannot copy to %d-th bit\n", dest_offset);
        return;
    }
    if(src_offset > 7 || src_offset < 0){
        printf("ERROR: Cannot copy from %d-th bit\n", src_offset);
        return;
    }

    while(num_bits > 0){
        cpy_bit(dest, dest_offset, src, src_offset);
      
        dest_offset++;
        src_offset++;

        if(dest_offset > 7){
            dest++;
            dest_offset = 0;
        }
        if(src_offset > 7){
            src++;
            src_offset = 0;
        }

        num_bits--;
    }
}

bool check_bit(unsigned char src, int index){
    if(index > 7 || index < 0){
        printf("ERROR: Cannot check %d-th bit\n", index);
        return false;
    }
    unsigned char mask = 0b10000000 >> index;
    return (src & mask) != 0;
}