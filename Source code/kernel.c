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
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include "shell.h" //For shellUI()
#include "memorymanager.h" //For pages manipulation
#include "pcb.h" //For PCB struct definition
#include "cpu.h" //For runInCPU()
#include "ram.h" //For writing/reading RAM
#include "shellmemory.h" //For clearShellMemory()

extern int CPUavailable;

//Ready queue: 
typedef struct RQNode{
    PCB *pcb;
    struct RQNode *next;
}RQNode;
RQNode *head, *tail;

//Returns 1 if the last PCB was removed, and 0 otherwise. 
int terminateProgram(){
    PCB *pcb = head->pcb;
    for(int i = 0; i < 10; i++){
        if(pcb->pageTable[i] == -1){ continue; }
        //Reset space in RAM
        resetRam((pcb->pageTable[i] * 4), (pcb->pageTable[i] * 4) + 3);
        //Make frame available
        enqueueFrame(pcb->pageTable[i]);
    }
    //Delete the file in the backing store:
    char command[100];
    sprintf(command, "rm ./BackingStore/program%d.txt", pcb->PID);
    if(system(command) == -1){ 
        puts("Couldn't remove file in the backing store.");
        EXIT_FAILURE; 
    }
    if((head->next) == NULL){ //If last PCB in the queue,
        return 1;
    }
    else{ return 0; }
}

/*Returns:  1 if the last PCB was removed
            0 if the current PCB was removed but it's not the last
            2 if the PCB's page was loaded to RAM*/
int pageFault(){
    PCB *pcb = head->pcb;
    //Update PCB's page:
    pcb->PC_page++;
    //If the program is done, terminate.
    if(pcb->PC_page > pcb->pages_max){
        return terminateProgram(pcb);
    }
    //If program isn't done, check if the page is in the pageTable:
    if(pcb->pageTable[pcb->PC_page] != -1){
        //Then update pcb's offset and PC.
        pcb->PC_offset = 0;
        pcb->PC = pcb->pageTable[pcb->PC_page] * 4;
        return 2;
    }
    else{ //Else page is not in the page table
        //Find the program in the backing store:
        char filename[50];
        sprintf(filename, "./BackingStore/program%d.txt", pcb->PID);
        FILE *program = fopen(filename, "r");
        if(program == NULL){ 
            puts("Couldn't open file in backing store.");
            EXIT_FAILURE;
        }
        //Find a frame:
        int frameNumber = dequeueFrame();
        int victim = 0; //Flag to test if there's a victim 
        if(frameNumber == -1){ 
            frameNumber = findVictim(pcb);
            victim = 1;
        }
        //Load the page in RAM:
        loadPage(pcb->PC_page, program, frameNumber);
        //Update page table:
        updatePageTable(pcb, pcb->PC_page, frameNumber, victim);
        //Update offset and PC:
        pcb->PC_offset = 0;
        pcb->PC = pcb->pageTable[pcb->PC_page] * 4;
        //Close file in the backing store
        fclose(program);
        return 2;
    }
}

/*Helper method for scheduler. 
mode = 0: Update queue without removing PCB
mode = 1: Update queue and remove PCB*/
void updateReadyQueue(int mode){
    if(mode == 0){
        RQNode *oldHead = head;
        head = head->next;
        oldHead->next = NULL; 
        tail->next = oldHead;
        tail = oldHead;
    }
    else if(mode == 1){
        RQNode *oldHead = head;
        head = head->next;
        free(oldHead->pcb);
        free(oldHead);
    }
    else{
        puts("Wrong mode in updateReadyQueue.");
    }
}

void scheduler(){
    //Check if CPU is available:
    if(!CPUavailable){
        puts("CPU not available.");
        return;
    }
    int runOutput, faultOutput;
    while(head != NULL){ //Loop while queue is not empty
        PCB *headPCB = head->pcb;
        //Set CPU's IP:
        headPCB->PC = (headPCB->pageTable[headPCB->PC_page] * 4) + headPCB->PC_offset;
        setAddress_CPU(headPCB->PC, headPCB->PC_offset);
        runOutput = runInCPU(2);
        //Check if there's a page fault
        if(runOutput == -2){ 
            faultOutput = pageFault(); 
            if(faultOutput == 1){ //All programs have terminated, so exit
                free(head->pcb);
                head = NULL;
                resetRam(0, 39);
                return; 
            }
            else if(faultOutput == 0){ //The current program terminates, continue with the next one
                updateReadyQueue(1);
                continue;
            }
            else{ //Page was loaded, update the queue
                if(head != tail){
                    updateReadyQueue(0);
                }
                continue;
            }
        } 
        else if(runOutput == -3){ //If the end of the program is reached,
            if(terminateProgram()){ //All programs have terminated, so exit
                free(head->pcb);
                head = NULL;
                resetRam(0, 39);
                return; 
            }
            else{ //Else just update ready queue
                updateReadyQueue(1);
            }
        }
        else{ //There was no quit, update the queue
            headPCB->PC_offset += 2;
            if(head != tail){ //We only update the queue when
                              //there's more than one PCB
                //Update ready queue
                updateReadyQueue(0);
            } 
        }
    }
} 

//Adds the PCB to the tail of the ready queue:
void addToReady(PCB *toAdd){
    //Check input pointer:
    if(toAdd == NULL){ return; }
    RQNode *newNode = (RQNode *)malloc(sizeof(RQNode));
    if(newNode == NULL){
        puts("Failed to add PCB to ready queue.");
        return;
    }
    newNode->pcb = toAdd;
    if(head == NULL){ //There was nothing in the queue 
        head = newNode, tail = newNode;
        head->next = NULL, tail->next = NULL;
    }
    else{
        newNode->next = NULL;
        tail->next = newNode;
        tail = newNode;
    }
}

int kernel(){
    int error;
    //Initialize CPU
    initCPU();
    //Initialize queue of frames
    initQueueFrame();
    //Instantiate ready queue pointers
    head = NULL, tail = NULL;
    //Call shellUI()
    error = shellUI();
    //When user quits, free ready queue pointers
    free(head), free(tail);
    //Clean up memory:
    clearShellMemory();
    return error;
}

//Returns 0 when the boot is successful, -1 otherwise.
int boot(){
    //Set all RAM slots to NULL
    if(resetRam(0, 39) == -1){ return -1; }
    DIR* backStore = opendir("BackingStore");
    //Clear the old BackingStore if there was one (and its contents)
    if(backStore){
        if(system("rm -r BackingStore") == -1){ return -1; }
    }
    //Create new BackingStore
    if(system("mkdir BackingStore") == -1){ return -1; }
    return 0;
}

int main(){ 
    int error = 0;
    error = boot(); //Perform the OS boot sequence
    if(error == -1){ 
        puts("Boot failed.");
        return -1; 
    }
    error = kernel(); //kernel does its thing
    return error;
}