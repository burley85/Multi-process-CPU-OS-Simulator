#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "configs.h"

#define MAX_LINE_LENGTH 1024


void create_config_file(){
    FILE *fp = fopen(INI_PATH, "w");
    if(fp == NULL){
        printf("ERROR: Failed to create config file.");
        return;
    }
    fprintf(fp, DEFAULT_CONFIG_TXT);

    fclose(fp);
}

char* get_current_line(){
    static char line[MAX_LINE_LENGTH + 1];

    return line;
}

char* get_next_line(FILE *fp){
    char line[MAX_LINE_LENGTH + 1];

    memset(line, 0, MAX_LINE_LENGTH + 1);
    fgets(line, MAX_LINE_LENGTH + 1, fp);
    if(feof(fp)) return NULL;
    if(line[MAX_LINE_LENGTH] != '\0')
        printf("ERROR: Maximum line length of %d exceeded in config file.\n", MAX_LINE_LENGTH);

    //Replace ; (comments) and \n with null terminator
    char *p = strchr(line, ';');
    while(p != NULL){
        *p = '\0';
        p = strchr(p, ';');
    }
    p = strchr(line, '\n');
    if(p != NULL) *p = '\0';


    //Replace trailing whitespace with null terminator
    int len = strlen(line);
    while(isspace(line[len - 1])){
        line[--len] = '\0';
    }

    //Skip leading whitespace
    p = line;
    while(isspace(p[0])) p++;

    //Skip empty lines
    if(strlen(line) == 0) return get_next_line(fp);


    char* old_line = get_current_line();
    memcpy(old_line, p, strlen(p));
    old_line[strlen(p)] = '\0';

    return old_line;
}

void get_key_and_value(FILE *fp, char **keyPtr, char **valuePtr){
    char* value;
    char* key = get_current_line();
    
    value = strchr(key, '=');
    if(value == NULL){
        printf("ERROR: found key without value in configs: %s\n", key);
        key = NULL;
        return;
    }

    *value = '\0'; //Split key and value
    value++; //Skip the '='

    //Trim trailing whitespace from key
    int len = strlen(key);
    while(isspace(key[len - 1])) key[--len] = '\0' ;
    

    //Trim leading whitespace from value
    while(isspace(*value)) value++;
    
    *keyPtr = key;
    *valuePtr = value;
}

//Returns 0 if the first len characters of s1 and s2 are equal, ignoring case
//Returns nonzero otherwise
int strcasencmp(char* s1, char* s2, int len){
    int pos = 0;
    while(pos < len){
        char c1, c2;

        c1 = tolower(s1[pos]);
        c2 = tolower(s2[pos]);

        if(c1 != c2) return c1 - c2;
        
        pos++;
    }
    return 0;
}

char value_to_char(char* value){
    if(strlen(value) > 1){
        printf("WARNING: Value '%s' does not match the expected type of char.\n", value);
        return 0;
    }
    else return *value;
}

char* value_to_matched_string(char* value, char** options, int num_options){
    for(int i = 0; i < num_options; i++){
        if(!strcasecmp(value, options[i])) return options[i];
    }
    printf("WARNING: Value '%s' does not match any of the following: ", value);
    for(int i = 0; i < num_options; i++){
        printf("'%s'", options[i]);
        if(i != num_options - 1) printf(", ");
    }
    printf(".\n");
    return NULL;
}

char** value_to_str_list(char* value, char separator, int* count){
    int value_len = strlen(value);

    //Count the number of substrings and split them with null terminators
    *count = 1;
    for(int i = 0; i < value_len - 1; i++){
        if(value[i] == separator){
            (*count)++;
            value[i] = '\0';
        }
    }

    char** list = malloc(sizeof(char*) * (*count));
    if(list == NULL){
        printf("ERROR: Failed to allocate memory in %s\n", __FUNCTION__);
        return NULL;
    }

    //Copy each of the substrings into dynamically allocated memory
    for(int i = 0; i < *count; i++){
        list[i] = malloc(strlen(value) + 1);
        if(list[i] == NULL){
            printf("ERROR: Failed to allocate memory in %s\n", __FUNCTION__);
            return NULL;
        }
        strcpy(list[i], value);
        list[i][strlen(list[i])] = '\0';
        value += strlen(list[i]) + 1;
    }

    return list;
}

void parse_simulator_section(struct config *configs, FILE* fp){
    char* line = get_next_line(fp);
    while(line[0] != '[' && !feof(fp)){
        char *key, *value;

        get_key_and_value(fp, &key, &value);
        if(strlen(key) > 0){
            if(!strcasecmp(key, "breakpointLabels")) configs->breakpoints = value_to_str_list(value, ',', &configs->breakpointCount);
            else if(!strcasecmp(key, "startingMode")){
                char *modes[] = {"continuous", "step"};
                char *mode = value_to_matched_string(value, modes, sizeof(modes) / sizeof(modes[0]));
                if(mode != NULL && !strcasecmp(mode, "continuous")) configs->startingMode = CONTINUOUS;
                else if(mode != NULL && !strcasecmp(mode, "step")) configs->startingMode = STEP;
                else printf("WARNING: Unknown starting mode '%s'\n", value);
            }
        }
        line = get_next_line(fp);
    }
} 

void parse_monitor_section(struct config *configs, FILE* fp){
    char* line = get_next_line(fp);
    while(line != NULL && line[0] != '[' && !feof(fp)){
        char *key, *value;
        
        get_key_and_value(fp, &key, &value);
        if(strlen(value) > 0){
            if(!strcasecmp(key, "runkey")) configs->runKey = value_to_char(value);
            else if(!strcasecmp(key, "stepkey")) configs->stepKey = value_to_char(value);
            else if(!strcasecmp(key, "quitkey")) configs->quitKey = value_to_char(value);
            
            else printf("WARNING: Unknown config key '%s'\n", key);
        }
        line = get_next_line(fp);
    }
}

void parse_config_file(struct config *configs, FILE* fp){
    char* section_names[] = {"simulator", "simmonitor"};
    void (*parse_functions[])(struct config *config, FILE *fp) = {parse_simulator_section, parse_monitor_section};

    char* line = get_next_line(fp);

    while(!feof(fp)){
        if(line[0] != '[' || line[strlen(line) - 1] != ']'){
            printf("WARNING: Unexpected line in config: '%s'\n", line);
            line = get_next_line(fp);
            continue;
        }
        int i;
        for(i = 0; i < (sizeof(section_names) / sizeof(section_names[0])); i++){
            if(!strcasencmp(section_names[i], line + 1, strlen(section_names[i]))){
                parse_functions[i](configs, fp);
                break;
            }
        }

        if(i == (sizeof(section_names) / sizeof(section_names[0]))){
            printf("WARNING: Unrecognized section in config: '%.*s'\n", (int) strlen(line) - 2, line + 1);
            line = get_next_line(fp);
        }
    }

}

struct config load_configs(){
    errno = 0;
    FILE *fp = fopen(INI_PATH, "r");
    if(fp == NULL && errno == ENOENT){
        printf("WARNING: Config file not found. Creating default config file.\n");
        create_config_file();
        fp = fopen(INI_PATH, "r");
    } 
    if(fp == NULL){
        printf("ERROR: Failed to open config file. Using defaults.\n");
        return DEFAULT_CONFIG;
    }
    
    struct config configs = DEFAULT_CONFIG;
    parse_config_file(&configs, fp);

    fclose(fp);

    return configs;
}