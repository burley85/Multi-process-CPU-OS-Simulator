#ifndef SIM_H_
#define SIM_H_

#include "cpu.h"

typedef struct sim{
    cpu cpu;
    enum modes {CONTINUOUS, STEP, EXIT} mode;
    bool running;
} sim;

sim* get_sim();
void reset_sim(sim* s);
void run_sim(sim* s);

#endif