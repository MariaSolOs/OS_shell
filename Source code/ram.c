//ram.c
#include <stdio.h>
#include <stdlib.h>

/*ram is an array of strings of size 40. Each 4 cells of the array
represent a frame.*/
char *ram[40];
int frameQueue[10]; //Keeps track of available frames

//Queue of frames implementation:
void initQueueFrame(){
    //Make all frames available
    for(int i = 0; i < 10; i++){
        frameQueue[i] = i;
    }
}

/*Sets cells from start to end (both inclusive) to null.
Output:  0 succesful reset
         -1  unsuccesful reset*/
int resetRam(int start, int end){
    //Check for valid input:
    if(start < 0 || end > 39 || start > end) { return -1; }
    for(int i = start; i <= end; i++){
        if(ram[i] != NULL){ ram[i] = NULL; }
    }
    return 0;
}

//Returns 1 if enqueue is successful, 0 otherwise
int enqueueFrame(int frame){
    if(frame < 0 || frame > 9){ return 0; }
    for(int i = 0; i < 10; i++){
        if(frameQueue[i] == -1){
            frameQueue[i] = frame;
            return 1;
        }
    }
    return 0;
}

//Searches RAM for a frame
int dequeueFrame(){
    int toReturn = frameQueue[0];
    //Must update frame queue
    for(int i = 0; i < 9; i++){        
        frameQueue[i] = frameQueue[i + 1];
    }
    frameQueue[9] = -1;
    return toReturn;
}

