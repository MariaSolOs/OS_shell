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
    
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h" //Parser call interpreter()

extern int pipedInput;
extern int running;

/*Output  -3: User asked to quit
          -1: Command couldn't be completed.
          0: Success*/
int parse(char input[]){
    int i;
    //Remove all \n and \r characters
    char noCRLF[1000];
    for(i = 0; i < 1000; i++){
        if(input[i] == '\n' || input[i] == '\r'){
           noCRLF[i] = ' ';
        }
        else{
            noCRLF[i] = input[i];
        }
    }
    //Skip initial spaces
    for(i = 0; noCRLF[i] == ' ' && i < 1000; i++);
    if(noCRLF[i] == '\0') return -1; //input was a bunch of spaces
    char *words[100], word[200];
    int j, wordsInd = -1;
    //Construct array for interpreter
    while(noCRLF[i] != '\0' && i < 1000 && i < strlen(noCRLF)){
        //Skip initial spaces
        while(noCRLF[i] == ' ' && i < 1000) i++;
        for(j = 0; noCRLF[i] != '\0' && noCRLF[i] != ' ' && i < 1000; i++, j++){
            word[j] = noCRLF[i];
        }
        if(strlen(word) == 0) break; //True if input has spaces after the word
        word[j] = '\0';
        wordsInd++, i++;
        words[wordsInd] = strdup(word);
        strcpy(word, ""); //Reset word buffer
    }
    //Number of arguments = wordsInd + 1
    return interpreter(words, wordsInd + 1);
}

int shellUI(){
    char userInput[1000];
    int errorCode, i;
    char *commandLine;
    puts("Kernel 2.0 loaded!");
    puts("Welcome to the Maria Solano's shell!");
    puts("Shell version 3.0 Updated March 2020");
    while(1){ //Loop until user wants to quit
        printf("$ ");
        commandLine = fgets(userInput, 999, stdin);
        //Skip initial spaces
        for(i = 0; userInput[i] == ' ' && i < 1000; i++);
        if(!isatty(fileno(stdin)) && !running){ pipedInput = 1; }
        //In case we must redirect input to the terminal again,
        if(commandLine == NULL) { 
            freopen("/dev/tty", "r", stdin); 
            break;
        }
        if((userInput[i] == '\n' || userInput[i] == '\r') && commandLine != NULL){
            if(pipedInput && !running){ printf("\n"); }
            continue; //Command is ""
        }
        errorCode = parse(userInput); //Parse the input
        if(errorCode == -3) break; //User asked to quit   
    }
    return 0;
}