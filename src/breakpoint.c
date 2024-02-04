#include <stdlib.h>

#include "breakpoint.h"

void add_breakpoint(sim* sim, bool (*condition_function) (cpu *, void *), void *param_ptr){
    breakpoint *bp = (breakpoint *) malloc(sizeof(breakpoint));

    bp->condition_function = condition_function;
    bp->param_ptr = param_ptr;

    appendDynamicArray(&(sim->breakpoints), bp);
}

bool check_breakpoints(sim* sim){
    for(int i = 0; i < sim->breakpoints.size; i++){
        breakpoint *bp = (breakpoint *) getDynamicArray(&(sim->breakpoints), i);
        if(bp->condition_function(&(sim->cpu), bp->param_ptr)) return true;
    }
    return false;
}