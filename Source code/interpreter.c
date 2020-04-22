/*  Copyright (C) 2020 Maria Solano

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.*/
    
#include <string.h>
#include <stdio.h>
#include "shellmemory.h" //For accessShellMemory()
#include "kernel.h" //For scheduler()
#include "shell.h" //For parse()
#include "memorymanager.h" //For loader()

int pipedInput = 0; //Used for avoiding $$$$ when having piped input
int running = 0; //Flag for running/executing a script

void help(){
    puts("Commands supported by this shell:");
    puts("help - Displays all the commands.");
    puts("quit - Exits/terminates the shell with \"Bye!\".");
    puts("set VAR VAL - Assigns VAL to VAR in shell memory.");
    puts("print VAR - Displays the value assigned to VAR.");
    puts("run SCRIPT.TXT - Executes the file SCRIPT.TXT");
    puts("exec p1 p2 p3 - Executes concurrent programs.");
}

int quit(){
    puts("Bye!");
    return -3; //-3 is the unique error code for quitting.
    //The actual quitting (exiting the while loop) is done in main
}

void run(char *fileName){
    running = 1;
    int error = 0;
    char line[1000];
    FILE *f = fopen(fileName, "rt");
    if(f == NULL){
        puts("Script not found.");
        running = 0;
        return;
    }
    fgets(line, 999, f);
    while(1){
        if(line[0] != '\n' && line[0] != '\r'){ //Also avoid blank lines
            error = parse(line);
            if(error){
                fclose(f);
                running = 0;
                return;
            }
        }
        if(feof(f)){ break; }
        fgets(line, 999, f); 
    }
    running = 0;
    fclose(f);
}

/*Output:   -1 exec command failed
            0 Success*/
int exec(char *programs[], int numPrograms){
    running = 1;
    for(int i = 0; i < numPrograms; i++){
        //Open each file:
        FILE *file = fopen(programs[i], "r");
        if(file == NULL){
            printf("Couldn't open file %s.\n", programs[i]);
            running = 0;
            return -1; 
        }
        //Launch the program:
        if(!launcher(file)){ 
            printf("Couldn't launch program %s\n", programs[i]);
            running = 0;
            return -1; 
        }
    }
    //Once everything's loaded, call the scheduler:
    scheduler();
    running = 0;
}

/*Assumes input is of the form COMMAND ARGUMENTS
Output    -3: User asked to quit
          -1: Command couldn't be completed.
          0: Success*/
int interpreter(char *words[], int numWords){
    char *command = words[0];
    int errorNumArgs = 0; //Flag for wrong number of arguments
    if(!strcmp(command, "help")){
        if(numWords == 1) help();
        else { errorNumArgs = 1; }
    }
    else if(!strcmp(command, "quit")) {
        if(numWords == 1){
            return quit();
        }
        else{ errorNumArgs = 1; }
    }
    else if(!strcmp(command, "set")){
        if(numWords == 3){ 
            //Use shell memory API:
            int output = accessShellMemory(words[0], words[1], words[2]); 
            if(output == -1){
                puts("set failed.");
                return -1;
            }
            if(pipedInput && !running){ 
                printf("\n"); 
                pipedInput = 0;
            }
        }
        else{ errorNumArgs = 1; }
    }
    else if(!strcmp(command, "print")){
        if(numWords == 2){
            //Use shell memory API:
            accessShellMemory(words[0], words[1], NULL); 
        }
        else{ errorNumArgs = 1; }
    }
    else if(!strcmp(command, "run")){
        if(numWords == 2){ run(words[1]); }
        else{ errorNumArgs = 1; }
    }
    else if(!strcmp(command, "exec")){
        int r = 0; //To store exec output
        //exec takes 1 to 3 arguments:
        if(numWords == 2){
            char *processes[] = {words[1]};
            r = exec(processes, 1); 
        }
        else if(numWords == 3){
            char *processes[] = {words[1], words[2]};
            r = exec(processes, 2); 
        }
        else if(numWords == 4){
            char *processes[] = {words[1], words[2], words[3]};
            r = exec(processes, 3); 
        }
        else{ errorNumArgs = 1; }
        if(r == -1){ return -1; }
    }
    else{
        puts("Unknown command.");
        return -1;
    }
    if(errorNumArgs){
        puts("Wrong number of arguments"); 
        return -1;
    }
    else{ return 0; }//No errors
}