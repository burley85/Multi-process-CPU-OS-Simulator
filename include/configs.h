#ifndef CONFIGS_H
#define CONFIGS_H

#include <stdlib.h>

//Parses a ini file
#define INI_PATH "../sim_config.cfg"
#define DEFAULT_CONFIG (struct config) {.breakpoints = NULL, .startingMode = STEP,\
                                        .runKey = 'r', .stepKey = 's', .quitKey = 'q'}

enum simMode{
    CONTINUOUS, STEP, EXIT, LOAD
};

struct config{
    char** breakpoints;
    enum simMode startingMode;
    char runKey;
    char stepKey;
    char quitKey;
};

struct config parseConfigs();

#endif