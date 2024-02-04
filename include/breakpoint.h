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


#endif