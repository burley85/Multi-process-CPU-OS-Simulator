#ifndef DECODING_H
#define DECODING_H

#include "helper.h"
#include "symbol_map.h"

char* decode_instruction(unsigned char* encoded_instruction, int* encoding_length, symbol_map symbols);

#endif