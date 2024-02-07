#include <stdlib.h>

#include "breakpoint.h"

bool rip_equal(cpu *c, void *test_val_ptr){
    unsigned long long test_val = *((unsigned long long*) test_val_ptr);
    if(c->rip == test_val){
        return true;
    }
    return false;
}

void add_breakpoint_at_label(sim* s, char* label){
    for(int i = 0; i < s->labels.size; i++){
        char* label_test = *((char**) getDynamicArray(&(s->labels), i));
        if(strcmp(label_test, label) == 0){
            unsigned long long* label_address_ptr = (unsigned long long*) getDynamicArray(&(s->label_addresses), i);
            printf("Adding breakpoint at 0x%llx\n", *label_address_ptr);
            add_breakpoint(s, rip_equal, (void*) label_address_ptr);
        }
    }
}

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