#ifndef CONFIGS_H
#define CONFIGS_H

#include <stdlib.h>

//Parses a ini file
#define INI_PATH "configs.ini"
#define DEFAULT_CONFIG_TXT "\
[Simulator]\n\
breakpointLabels=\n\
startingMode=CONTINUOUS\n\n\
[SimMonitor]\n\
runKey=r\n\
stepKey=s\n\
quitKey=q\n\
"

#define DEFAULT_CONFIG (struct config) {.breakpoints = NULL, .breakpointCount = 0, .startingMode = CONTINUOUS,\
                                        .runKey = 'r', .stepKey = 's', .quitKey = 'q'}

enum simMode{
    CONTINUOUS, STEP, EXIT, LOAD
};

struct config{
    char** breakpoints;
    int breakpointCount;
    enum simMode startingMode;
    char runKey;
    char stepKey;
    char quitKey;
};

struct config load_configs();

#endif