#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include "sim.h"
#include "helper.h"

typedef struct breakpoint{
    bool (*condition_function) (cpu *, void *);
    void *param_ptr;
} breakpoint;

void add_breakpoint(sim* sim, bool (*condition_function) (cpu *, void *), void *param_ptr);
bool check_breakpoints(sim* sim);
//Add breakpoints at every address that the label is found
void add_breakpoint_at_label(sim* s, char* label);

//Condition functions:

bool rip_equal(cpu *c, void *test_val_ptr);

#endif