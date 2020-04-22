//pcb.c
#include<stdlib.h>
#include<stdio.h>

typedef struct PCB{
    int pageTable[10]; //Index is the page number, value the frame number
    int PC; //Current position of the process (frame address + offset)
    int PC_page; //Page the program is currently at
    int PC_offset; //Corresponding page offset
    int pages_max; //Total number of pages in the program
    int PID; //Used to open programs in the backing store
}PCB;

//Creates a PCB instance
PCB *makePCB(int pages_max, int PID){
    PCB *added = malloc(sizeof(PCB));
    if(added == NULL){
        puts("Failed to create PCB.");
        return NULL;
    }
    //Initially no frames are assigned:
    for(int i = 0; i < 10; i++){
        added->pageTable[i] = -1;
    }
    added->PC = 0;
    added->PC_page = 0;
    added->PC_offset = 0;
    added->pages_max = pages_max;
    added->PID = PID;
    return added;
}