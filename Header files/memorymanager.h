//memorymanager.h

#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H
#include "pcb.h"
//Public functions:
int launcher(FILE *p);
void initQueueFrame();
int findVictim(PCB *pcb);
void loadPage(int pageNumber, FILE *f, int frameNumber);
void updatePageTable(PCB *pcb, int pageNumber, int frameNumber, int victimFrame);
#endif