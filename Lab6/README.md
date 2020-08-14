Group Members: Austin Ng, Kelechi Igwe

Instructions: Run make command using either 'make' or 'make allocator' to make
the executable for the program. Then, run ./allocator with the number of bytes
to initially allocate (Maximum is set at 16MB, so for example you can do
./allocator 1000000). Then use the four commands RQ, RL, C, and STAT to request
memory, release memory, compact memory, and report memory, respectively. Then
enter X to exit the program.

Who Did What:
Kelechi - validCommand(), freeMemory(), requestMemory() (RQ function),
 	  compactMemory() (C function)
Austin - printMemory() (STAT function), releaseMemory() (RL function)
