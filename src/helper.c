#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "helper.h"

#define DYNAMIC_ARRAY_MINIMUM_CAPACITY 2
#define DYNAMIC_ARRAY_GROWTH_FACTOR 2
DynamicArray createDynamicArray(int capacity, int item_size, bool free_items){
    //Check validity of defined values
    if(DYNAMIC_ARRAY_MINIMUM_CAPACITY < 1){
        printf("ERROR: DYNAMIC_ARRAY_DEFAULT_CAPACITY must be at least 1\n");
        exit(1);
    }
    if(DYNAMIC_ARRAY_GROWTH_FACTOR < 1){
        printf("ERROR: DYNAMIC_ARRAY_GROWTH_FACTOR must be at least 1\n");
        exit(1);
    }
    int capacity_after_first_grow = DYNAMIC_ARRAY_GROWTH_FACTOR * DYNAMIC_ARRAY_MINIMUM_CAPACITY;
    if(capacity_after_first_grow <= DYNAMIC_ARRAY_MINIMUM_CAPACITY){
        printf("ERROR: DynamicArray will never grow with the define values of DYNAMIC_ARRAY_GROWTH_FACTOR and DYNAMIC_ARRAY_DEFAULT_CAPACITY\n");
        exit(1);
    }

    DynamicArray a;
    
    a.item_size = item_size;
    a.size = 0;
    a.free_items = free_items;
    if(capacity < DYNAMIC_ARRAY_MINIMUM_CAPACITY) capacity = DYNAMIC_ARRAY_MINIMUM_CAPACITY;

    a.array = malloc(capacity * item_size);
    if(a.array == NULL){
        printf("ERROR: Could not allocate memory for array\n");
        a.capacity = 0;
    }
    else a.capacity = capacity;

    return a;
}

void destroyDynamicArray(DynamicArray* a){
    if(a->array == NULL) return;
    
    if(a->free_items){
        for(int i = 0; i < a->size; i++){
            free(*((char**)getDynamicArray(a, i)));
        }
    }
    free(a->array);

    a->array = NULL;
    a->size = 0;
    a->capacity = 0;
}

void* getDynamicArray(DynamicArray* a, int index){
    if(a < 0 || index >= a->size){
        printf("ERROR: Index %d out of bounds\n", index);
        return NULL;
    }
    return ((char*) a->array) + (index * a->item_size);
}

void* appendDynamicArray(DynamicArray* a, void* item){
    if(a == NULL || a->array == NULL || item == NULL){
        printf("ERROR: Failed to append to DynamicArray due to NULL pointer \n");
        return NULL;
    }

    if(a->size == a->capacity){
        int new_capacity = a->capacity * DYNAMIC_ARRAY_GROWTH_FACTOR;
        void* new_array = realloc(a->array, new_capacity * a->item_size);
        if(new_array == NULL){
            printf("ERROR: Could not reallocate memory for array\n");
            return NULL;
        }
        a->array = new_array;
        a->capacity = new_capacity;
    }

    void* item_location = ((char*) a->array) + (a->size * a->item_size);
    memcpy(item_location, item, a->item_size);

    a->size++;

    return item_location;
}

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

#define BUFFER_START_SIZE 2
#define BUFFER_GROWTH_FACTOR 2

char* fgettrimmedline(FILE* fp, int* buffer_size){
    int temp_size = BUFFER_START_SIZE;
    char* buffer = malloc(temp_size);
    memset(buffer, 0, temp_size);

    if(buffer == NULL){
        printf("ERROR: Could not allocate memory for buffer\n");
        return NULL;
    }

    while(1){
        char* rval = fgets(buffer + strlen(buffer), temp_size - strlen(buffer), fp);

        if(ferror(fp)){
            printf("ERROR: Could not read line\n");
            free(buffer);
            if(buffer_size != NULL) *buffer_size = 0;
            return NULL;
        }

        if(feof(fp) || buffer[strlen(buffer) - 1] == '\n') break;

        temp_size *= BUFFER_GROWTH_FACTOR;
        char* new_buffer = realloc(buffer, temp_size);
        if(new_buffer == NULL){
            printf("ERROR: Could not reallocate memory for buffer\n");
            free(buffer);
            if(buffer_size != NULL) *buffer_size = 0;
            return NULL;
        }
        buffer = new_buffer;
    }

    //Trim the whitespace from the beginning of the buffer
    int start = 0;
    while(buffer[start] == ' ' || buffer[start] == '\t' || buffer[start] == '\n') start++;
    if(start > 0) memmove(buffer, buffer + start, strlen(buffer) - start + 1);

    //Trim the whitespace from the end of the buffer
    int end = strlen(buffer) - 1;
    while(buffer[end] == ' ' || buffer[end] == '\t' || buffer[end] == '\n') end--;
    buffer[end + 1] = '\0';

    if(buffer_size != NULL) *buffer_size = temp_size;
    return buffer;
}