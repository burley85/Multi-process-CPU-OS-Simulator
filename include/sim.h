#ifndef SIM_H
#define SIM_H

#include "cpu.h"
#include "helper.h"

typedef struct sim{
    cpu cpu;
    enum modes {CONTINUOUS, STEP, EXIT} mode;
    bool running;
    DynamicArray breakpoints;
} sim;

sim* get_sim();
void reset_sim(sim* s);
void run_sim(sim* s);

#endif