//ram.h

#ifndef RAM_H
#define RAM_H
//Public functions:
int resetRam(int start, int end);
int dequeueFrame();
int enqueueFrame(int frame);
#endif