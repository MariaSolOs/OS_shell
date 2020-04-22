//cpu.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shell.h" //For parse()

extern char *ram[40];
int CPUavailable; //1 = CPU available, 0 = CPU not available

struct CPU{
    int IP; //IP (RAM address) = frame_address + offset
    int offset;
    char IR[1000]; //Currently executing instruction
    int quanta;
}CPU;

//Public APIs to read/set CPU fields:
void initCPU(){
    CPU.offset = 0;
    strcpy(CPU.IR, "");
    CPU.quanta = 0;
    CPUavailable = 1;
}
void setAddress_CPU(int IP, int offset){
    CPU.IP = IP;
    CPU.offset = offset;
}

/*Returns: 0 if the entire quanta was run
           -2 if there's a page fault
           -3 if the user quits or the program ends*/
static int run(int quanta){
    if(CPU.offset == 4 || CPU.IP < 0){ //Pagefault
        return -2; 
    }
    CPU.quanta = quanta; 
    int output;
    //Execute quanta instructions or until the end of the script
    while(CPU.quanta != 0){
        if(ram[CPU.IP] == NULL){ 
            return -3; 
        }
        strcpy(CPU.IR, ram[CPU.IP]);
        output = parse(CPU.IR);
        if(output == -3){ return -3; } //Check if program quits
        CPU.IP++, CPU.quanta--; CPU.offset++;
    }
    return 0;
}

//Public API to run a PCB in the CPU
int runInCPU(int quanta){
    //Check that quanta is valid.
    if(quanta < 0){
        puts("Invalid quanta input.");
        return -1;
    }
    CPUavailable = 0;
    int output = run(quanta);
    CPUavailable = 1;
    return output;
}
