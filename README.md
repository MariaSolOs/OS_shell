Hi there :)
Here's some stuff I think you should know about my shell. I've divided my 
comments into sections (according to which file they refer to). 

### GENERAL
1. Header files are included for all C files.
3. I include a nice makefile to compile this project. Make sure all C files and header 
files are in the same directory.
4. You'll obtain error messages when you run my testfile. This is exactly what I want,
I wish to how my shell won't break. 

### SHELL.C
1. If the user writes no command and presses enter, the prompt is displayed again.
2. If a file is passed as input through ./mykernel < file.txt, the shell reads each
line as input and exits at the end of the file.  
3. Spaces between arguments are ignored. So "set   x 2" and "   set x 2" are 
equivalent valid commands. The proof that my shell can handle this is shown with my
testfile. 
4. In shellUI() I used freopen("/dev/tty", "r", stdin) to redirect the input stream
back to the terminal to deal with the end of piped input (and so when I reach the 
end of the file, the prompt is displayed again, unless there was a quit command).

### SHELLMEMORY.C
1. The size of the shell's memory array is 1000. If the user tries to set a new
variable and memory is full, the set is unsuccesful and the "set failed." 
message is displayed. 
2. accessShellMemory is the public API for printing and setting variables in 
shell memory. clearShellMemory is used by the kernel at the end of the session. 

### INTERPRETER.C 
1. In exec(), either all the files are successfully executed or nothing runs at 
all. 
2. If a script contains the command "quit", then only the 
script terminates, not the entire shell session.

### KERNEL.C 
1. The ready queue, with a linked-list implementation, is in this C file. It made
sense to me to have it here since the scheduler is in this file. Note that my
head and tail pointers aren't actual PCB pointers but RQNode (Ready Queue Node) 
pointers.
2. After a process runs its quanta, I also check if the quit command was executed. If so,
I remove the program from the queue.
3. Note that when a program terminates, I also add all the frames it was using to the
available frames queue. This helps to avoid the search for victim frames.

### RAM.C 
1. I added the extra function resetRam to set RAM cells to NULL when needed.
2. My queue of frames implementation is in this file.

### CPU.C 
1. runInCPU is the public function called by kernel.c. The run function coded
inside cpu.c remains static (only accessible within this file). 
2. CPUavailable is used as a flag to test if the CPU is currently available. I 
also use it as a "mutex" in runInCPU. 

### MEMORYMANAGER.C 
1. In order to keep track of which PCB is using which frame, I have the array of PCB
pointers usingFrames.

Finally, please appreciate my kernel. I put a lot of effort into it and I'm kind of proud <3
