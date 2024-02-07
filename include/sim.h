#ifndef SIM_H
#define SIM_H

#include "cpu.h"
#include "helper.h"

typedef struct sim{
    cpu cpu;
    enum modes {CONTINUOUS, STEP, EXIT} mode;
    bool running;
    DynamicArray breakpoints;
    DynamicArray labels;
    DynamicArray label_addresses;
} sim;

sim* get_sim();
void reset_sim(sim* s);
void run_sim(sim* s);
void encode_file(FILE* fp, sim* sim, unsigned long long start_address);

#endif