//memorymanager.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pcb.h" //For PCB struct definition
#include "ram.h" //For queue of frames access
#include "kernel.h" //For addToReady()

extern char *ram[40];
int PID = 0; //Used for filenames in the Backing Store
PCB *usingFrames[10]; //Stores pointers to PCBs using each frame

/*Returns the total number of pages needed by the program
based on the number of '\n' characters*/
int countTotalPages(FILE *f){
    fseek(f, 0, SEEK_SET); 
    int numLines = 0;
    for(char c = getc(f); c != EOF; c = getc(f)){
        if (c == '\n'){ numLines++; }
    }
    if(numLines % 4 == 0){ return (numLines / 4); }
    else{ return (numLines / 4) + 1; }
}

void loadPage(int pageNumber, FILE *f, int frameNumber){
    //Check for valid frameNumber:
    if((frameNumber < 0) || (frameNumber > 9)){
        puts("Frame number is not valid");
        return;
    }
    //Reset the file pointer to the beginning
    fseek(f, 0, SEEK_SET); 
    int fileIndex = 0;
    char line[100];
    //Obtain first line of the page
    while(fileIndex <= (pageNumber * 4)){
        fgets(line, 99, f);
        //Be careful of reaching the end of the file:
        if(feof(f)){
            if(fileIndex == (pageNumber * 4)){
                break;
            }
            else { return; } 
        }
        fileIndex++;
    }
    int loadedLines = 0;
    int ramIndex = frameNumber * 4;
    //Clear up frame:
    resetRam(ramIndex, ramIndex + 3);
    while(loadedLines != 4){ 
        line[strlen(line)] = '\0';
        ram[ramIndex] = strdup(line);
        loadedLines++, ramIndex++;
        if(feof(f)){ break; }
        strcpy(line, "");
        fgets(line, 99, f);
    }
}

//Searches RAM for a frame
int findFrame(){
    return dequeueFrame();
}

/*Helper function for findVictim. Returns 1 if frame appears
in pageTable, 0 otherwise.*/ 
int inPageTable(int pageTable[], int frame){
    int inTable = 0;
    for(int i = 0; i < 10; i++){
        if(pageTable[i] == frame){
            inTable = 1;
            break;
        }
    }
    return inTable;
}

/*Updates the page table and the usingFrames array. Also updates
the victim page table if there was one.
*/
void updatePageTable(PCB *pcb, int pageNumber, int frameNumber, int victim){
    if(victim){ //Update victim page table
        int deletedPage;
        for(int i = 0; i < 10; i++){
            if((usingFrames[frameNumber]->pageTable)[i] == frameNumber){
                deletedPage = i;
                break;
            }
        }
        usingFrames[frameNumber]->pageTable[deletedPage] = -1;
    }
    pcb->pageTable[pageNumber] = frameNumber; //Update killer page table
    usingFrames[frameNumber] = pcb; 
}

//Returns the victim frame number 
int findVictim(PCB *pcb){
    //Pick a random frame number
    int victim = (rand() % 10);
    //If victim frame isn't in the page table, return it
    if(!inPageTable(pcb->pageTable, victim)){
        return victim; 
    }
    //Else increment frame until we find a victim
    while(inPageTable(pcb->pageTable, victim)){
        victim = (victim + 1) % 10;
    }
    return victim;
}

//Returns 1 if launch is successful, 0 otherwise.
int launcher(FILE *file){
    //Copy file to the backing store
    char filename[100];
    sprintf(filename, "./BackingStore/program%d.txt", PID);
    FILE *copy = fopen(filename, "w+");
    if(copy == NULL){ 
        puts("Failed to open file in the backing store.");
        return 0; 
    }
    char c = fgetc(file);
    while (c != EOF){ 
        fputc(c, copy); 
        c = fgetc(file); 
    } 
    //Close pointer to the original file.
    fclose(file);
    //Count the number of pages needed by the program:
    int numPages = countTotalPages(copy);
    if(numPages > 10){ 
        puts("Program doesn't fit in memory.");
        return 0; 
    }
    //Create the PCB:
    PCB *pcb = makePCB(numPages, PID);
    PID++;
    //Load pages and update page table
    int victim = 0; //Flag to test if there's a victim
    int frameNumber = findFrame();
    if(frameNumber == -1){ 
        frameNumber = findVictim(pcb); 
        victim = 1;
    }
    if(numPages <= 1){ //If program only needs 1 page,
        loadPage(0, copy, frameNumber);
        updatePageTable(pcb, 0, frameNumber, victim);
    }
    else{ //Else only load 2 pages
        loadPage(0, copy, frameNumber);
        updatePageTable(pcb, 0, frameNumber, victim);
        victim = 0;
        frameNumber = findFrame();
        if(frameNumber == -1){ 
            frameNumber = findVictim(pcb); 
            victim = 1;
        }
        loadPage(1, copy, frameNumber); 
        updatePageTable(pcb, 1, frameNumber, victim);
        victim = 0;
    }
    //Close the file in the backing store
    fclose(copy);
    //Set PCB's PC
    pcb->PC = (pcb->pageTable[0]) * 4;
    //Add PCB to the ready queue:
    addToReady(pcb);
    return 1;
}