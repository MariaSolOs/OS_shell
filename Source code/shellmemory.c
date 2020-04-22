//shellmemory.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct MEM{
    char *var;
    char *value;
}memory[1000];
//Global variable to keep track of used memory
int memoryIndex = 0;

//Returns 0 if set is successful and -1 otherwise.
int set(char *var, char *val){
    for(int i = 0; i < memoryIndex && memoryIndex < 999; i++){
        //If var already exists, update its value
        if(!strcmp(memory[i].var, var)){
            memory[i].value = strdup(val);
            return 0;
        }
    }
    if(memoryIndex < 999){
        //If not present, add entry to memory
        memory[memoryIndex].var = strdup(var);
        memory[memoryIndex].value = strdup(val);
        memoryIndex++;
        return 0;
    }
    return -1; //Unsuccesful set
}

/*Output: 0 if variable exists (and prints its value)
          -1 if variable doesn't exist (and prints error message)*/
int print(char *var){
    for(int i = 0; i < memoryIndex && memoryIndex < 999; i++){
        //If var exists, print its value
        if(!strcmp(memory[i].var, var)){
            printf("%s\n", memory[i].value);
            return 0;
        }
    }
    puts("Variable does not exist.");  
    return -1;
}

//The following is the shell memory public API
int accessShellMemory(char *command, char *var, char *val){
    if(!strcmp(command, "set")){
        return set(var, val);
    }
    else if(!strcmp(command, "print")){
        return print(var);
    }
    else{ return -1; }
}

//Called when user quits the shell
void clearShellMemory(){
    for(int i = 0; i <= memoryIndex; i++){
        free(memory[i].var);
        free(memory[i].value);
    }
}

