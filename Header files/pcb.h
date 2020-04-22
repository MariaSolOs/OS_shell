//pcb.h

#ifndef PCB_H
#define PCB_H
//Public variable:
typedef struct PCB{
    int pageTable[10]; 
    int PC;
    int PC_page; 
    int PC_offset;
    int pages_max; 
    int PID;
}PCB;
//Public function:
PCB *makePCB(int pages_max, int PID);
#endif