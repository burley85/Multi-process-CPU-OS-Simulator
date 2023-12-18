#ifndef HELPER_H
#define HELPER_H

#include <stdbool.h>

typedef struct DynamicArray{
    void* array;
    int item_size;
    int size;
    int capacity;
    bool free_items;
} DynamicArray;

DynamicArray createDynamicArray(int capacity, int item_size, bool free_items);
void destroyDynamicArray(DynamicArray* array);
void* getDynamicArray(DynamicArray* array, int index);
void* appendDynamicArray(DynamicArray* array, void* item);

void print_bin(unsigned char* buffer, int length);
void cpy_bit(unsigned char* dest, unsigned char dest_offset, unsigned char* src, unsigned char src_offset);
void memcpy_bits(void* dest, unsigned char dest_offset, void* src, unsigned char src_offset, int num_bits);
bool check_bit(unsigned char src, int index);
char* fgettrimmedline(FILE* fp, int* buffer_size);

#endif