#ifndef HELPER_H
#define HELPER_H

void print_bin(unsigned char* buffer, int length);
void cpy_bit(unsigned char* dest, unsigned char dest_offset, unsigned char* src, unsigned char src_offset);
void memcpy_bits(void* dest, unsigned char dest_offset, void* src, unsigned char src_offset, int num_bits);

#endif