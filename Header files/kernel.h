//kernel.h

#ifndef KERNEL_H
#define KERNEL_H
#include "pcb.h"
//Public functions:
void scheduler();
void addToReady(PCB *toAdd);
#endif