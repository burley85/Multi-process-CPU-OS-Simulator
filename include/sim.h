#ifndef SIM_H
#define SIM_H

#include "cpu.h"
#include "helper.h"
#include "configs.h"

#define LABEL_MAP_FILENAME "symbols.o"

typedef struct sim{
    cpu cpu;
    enum simMode mode;
    bool running;
    DynamicArray breakpoints;
    DynamicArray labels;
    DynamicArray label_addresses;
    int label_count;
} sim;

sim* get_sim();
void reset_sim(sim* s);
void run_sim(sim* s, struct config configs);
void encode_file(FILE* fp, sim* sim, unsigned long long start_address);

#endif