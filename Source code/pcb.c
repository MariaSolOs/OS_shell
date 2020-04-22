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