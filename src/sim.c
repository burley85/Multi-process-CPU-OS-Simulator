#include <Windows.h>
#include "sim.h"
#include "breakpoint.h"
#include "encoding.h"
#include "symbol_map.h"
#include "configs.h"

#define OBJ_NAME "SimCPUObj"

//Sim gets reset every time main program is run or object is created
void reset_sim(sim* s){
    memset(&(s->cpu), 0, sizeof(cpu));
    s->mode = LOAD;
    s->breakpoints = createDynamicArray(0, sizeof(breakpoint), true);
    
    //Clear label map
    s->labels = createDynamicArray(0, sizeof(char*), true);
    s->label_addresses = createDynamicArray(0, sizeof(unsigned long long), false);
    FILE *fp = fopen(LABEL_MAP_FILENAME, "w");
    if(fp != NULL) fclose(fp);
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

void run_sim(sim* s, struct config configs){
    cpu* cpu = &(s->cpu);
    s->mode = configs.startingMode;
    s->running = true;
    
    while(1){
        execute_current_instruction(cpu);

        if(s->mode == STEP || check_breakpoints(s)) s->running = false;
        while(!s->running){}
    }
}

//Replaces #define keys with their values, and removes comments
//Returns a pointer to a temporary file containing the preprocessed code
FILE* preprocess_file(FILE* fp){
    FILE* temp = tmpfile();
    if(temp == NULL){
        printf("ERROR: Failed to create temporary file\n");
        exit(1);
    }

    DynamicArray keys = createDynamicArray(0, sizeof(char*), true);
    DynamicArray values = createDynamicArray(0, sizeof(char*), true);

    while(!feof(fp)){
        char* line = fgettrimmedline(fp, NULL);
        if(line == NULL) exit(1);

        //Replace any comments with a null terminator
        char* replace = strchr(line, ';');
        if(replace != NULL){
            replace--;
            while(*replace == ' ' || *replace == '\t' ) replace--;
            *(replace + 1) = '\0';
        }

        //Ignore empty lines
        if(strlen(line) == 0){
            free(line);
            continue;
        }

        //Check if the line is a #define
        if(strncmp(line, "#define ", 8) == 0){
            char* key = malloc(strlen(line) + 1);
            char* value = malloc(strlen(line) + 1);
            memset(key, 0, strlen(line) + 1);
            memset(value, 0, strlen(line) + 1);

            //Get key (format: [A-Za-z_][A-Za-z_0-9]*) and value (rest of line)
            sscanf(line, "#define %[A-Za-z0-9_] %[^\n]", key, value);
            if(strlen(key) == 0 || (key[0] >= '0' && key[0] <= '9')){
                printf("ERROR: Invalid preprocessor command '%s'\n", line);
                exit(1);
            }

            if(appendDynamicArray(&keys, &key) == NULL || appendDynamicArray(&values, &value) == NULL){
                printf("ERROR: Failed to add key-value pair to symbol table\n");
                exit(1);
            }
            free(line);
            continue;
        }
        
        else{
            //Iterate through each word in the line and check if it is a key
            int line_pos = 0;
            while(line_pos < strlen(line)){

                char first_char = line[line_pos];
                //If first character is in [A-Za-Z0-9_] then read until next character not in [A-Za-Z0-9_]
                if((first_char >= 'A' && first_char <= 'Z') || (first_char >= 'a' && first_char <= 'z') || first_char == '_'){
                    char* str = malloc(strlen(line) + 1);
                    memset(str, 0, strlen(line) + 1);
                    sscanf(line + line_pos, "%[A-Za-z0-9_]", str);
                    line_pos += strlen(str);
                    
                    //Check if the string is a key (can't be a key if first character is in [0-9])
                    bool is_key = false;
                    if(!(str[0] >= '0' && str[0] <= '9')){  
                        for(int i = 0; i < keys.size; i++){
                            char* key = *((char**) getDynamicArray(&keys, i));
                            if(strcmp(key, str) == 0){
                                free(str);
                                str = *((char**) getDynamicArray(&values, i));
                                is_key = true;
                                break;
                            }
                        }
                    }
                    //Print the string to the file
                    fprintf(temp, "%s", str);
                    if(!is_key) free(str);
                }
                //If first character is in [0-9] then read until next character not in [A-Za-z0-9_]
                else if(first_char >= '0' && first_char <= '9'){
                    char* str = malloc(strlen(line) + 1);
                    memset(str, 0, strlen(line) + 1);
                    sscanf(line + line_pos, "%[A-Za-z0-9_]", str);
                    line_pos += strlen(str);

                    //Print the string to the file
                    fprintf(temp, "%s", str);
                    free(str);
                }
                //Otherwise, print that character to the file
                else{
                    fprintf(temp, "%c", first_char);
                    line_pos++;
                }
            }
            fprintf(temp, "\n");
        }
    }

    destroyDynamicArray(&keys);
    destroyDynamicArray(&values);
    fclose(fp);
    
    rewind(temp);
    return temp;
}

void encode_file(FILE* fp, sim* sim, unsigned long long start_address){
    cpu* cpu = &(sim->cpu);

    fp = preprocess_file(fp);
    if(fp == NULL){
        printf("ERROR: Failed to preprocess file\n");
        exit(1);
    }

    DynamicArray *labels = &(sim->labels);
    DynamicArray *label_addresses = &(sim->label_addresses);
    FILE* label_fp = fopen(LABEL_MAP_FILENAME, "a");
    //Initial pass to look for labels and find their addresses
    unsigned long long address = start_address;
    while(!feof(fp)){
        char* instruction = fgettrimmedline(fp, NULL);
        if(instruction == NULL) exit(1);

        int instruction_length = strlen(instruction);

        //Check if the line is a label (ends with a colon)
        if(instruction[instruction_length - 1] == ':'){
            //Make sure label fits the following regex: [A-Za-z_][A-Za-z1-9_]+:
            if(instruction_length == 1){
                printf("ERROR: Label cannot be empty\n");
                exit(1);
            }
            if((instruction[0] < 'A' || instruction[0] > 'Z') && (instruction[0] < 'a' || instruction[0] > 'z') && instruction[0]!= '_'){
                printf("ERROR: Label '%s' starts with invalid character\n", instruction);
                exit(1);
            }

            for(int i = 1; i < instruction_length - 1; i++){
                if((instruction[i] < 'A' || instruction[i] > 'Z') && (instruction[i] < 'a' || instruction[i] > 'z') && (instruction[i] < '1' || instruction[i] > '9') && instruction[i] != '_'){
                    printf("ERROR: Label '%s' contains invalid characters\n", instruction);
                    exit(1);
                }
            }

            char* label = malloc(strlen(instruction) + 1);
            memset(label, 0, strlen(instruction) + 1);
            strncpy(label, instruction, instruction_length);
            label[instruction_length - 1] = '\0';
            //Add the label to the symbol table
            if(appendDynamicArray(labels, &label) == NULL
                    || appendDynamicArray(label_addresses, &address) == NULL
                    || fprintf(label_fp, "%s %llu\n", label, address) < 0){
                printf("ERROR: Failed to add label to symbol table\n");
                exit(1);
            }
            sim->label_count++;
        }
        else{
            int encoding_length;
            unsigned char* encoding = encode_instruction(instruction, &encoding_length);
            address += encoding_length;
            if(encoding != NULL) free(encoding);
        }
    }

    fseek(fp, 0, SEEK_SET);
    address = start_address;

    while(!feof(fp)){
        char* instruction = fgettrimmedline(fp, NULL);
        int encoding_length = 0;

        unsigned char* encoding = encode_instruction(instruction, &encoding_length);
        if(encoding != NULL){
            //If instruction is a jump or a call, replace the 8 0 bytes with the address of the label
            bool instruction_is_jump = ((encoding[0] >> 4) >= JMP_ENCODING && (encoding[0] >> 4) <= JNS_ENCODING && !(encoding[0] >= KERNEL_COMMAND_ENCODING));
            bool instruction_is_call = (encoding[0] == PUSH_RIP_ENCODING) && (encoding[1] >> 4) == JMP_ENCODING;
            if(instruction_is_jump || instruction_is_call){
                //Find the label
                char* label = malloc(strlen(instruction) + 1);
                memset(label, 0, strlen(instruction) + 1);
                sscanf(instruction, "%*s %s", label);
                int i;
                for(i = 0; i < labels->size; i++){
                    char* label_test = *((char**) getDynamicArray(labels, i));

                    if(strcmp(label_test, label) == 0){
                        unsigned long long label_address = *((unsigned long long*) getDynamicArray(label_addresses, i));
                        if(label_address >= start_address){
                            if(instruction_is_jump) memcpy(encoding + 1, &label_address, 8);
                            else if(instruction_is_call) memcpy(encoding + 2, &label_address, 8);
                            break;
                        }
                    }
                }
                if(i == labels->size){
                    printf("ERROR: Label '%s' not found\n", label);
                    exit(1);
                }
            }
            write_memory(cpu, address, encoding, encoding_length);
            address += encoding_length;
            free(encoding);
        }
    }
    fclose(label_fp);
}