//cpu.h

//Public functions: 
#ifndef CPU_H
#define CPU_H
//Public functions: 
void setAddress_CPU(int IP, int offset);
void initCPU();
int runInCPU(int quanta);
#endif
