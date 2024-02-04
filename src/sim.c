#include <Windows.h>
#include "sim.h"
#include "breakpoint.h"

#define OBJ_NAME "SimCPUObj"
#define SIM_MODE STEP

//Sim gets reset every time main program is run or object is created
void reset_sim(sim* s){
    memset(&(s->cpu), 0, sizeof(cpu));
    s->mode = SIM_MODE;
    s->breakpoints = createDynamicArray(0, sizeof(breakpoint), true);
}

sim* get_sim(){
    HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, OBJ_NAME);
    bool fileExisted = (hMapFile != NULL);
    if(!fileExisted){
        //Create the object
        int maxSizeHigh = sizeof(sim) >> 32;
        int maxSizeLow = sizeof(sim) - maxSizeHigh;
        hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, maxSizeHigh, maxSizeLow, OBJ_NAME);
        if(hMapFile == NULL){
            printf("ERROR: Failed to share sim object\n");
            exit(1);
        }
    }


    sim* simPtr = (sim*) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(sim));
    if(simPtr == NULL){
        printf("ERROR: Failed to map sim object\n");
        exit(1);
    }

    if(!fileExisted) reset_sim(simPtr);

    return simPtr;  
}

void run_sim(sim* s){
    cpu* cpu = &(s->cpu);
    s->running = true;
    
    while(1){
        execute_current_instruction(cpu);

        if(s->mode == STEP || check_breakpoints(s)) s->running = false;
        while(!s->running){}
    }
}