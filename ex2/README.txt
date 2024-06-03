ahmad_dall7,areen0507
Ahmad Dallashe (324059856), Areen Mansour (212900211)
EX: 1


FILES:
osm.cpp 
graph.png
Makefile
README.txt

REMARKS:
-

ANSWERS:
Assignment 1 :

The program is executing the binary file "WhatIDo".
 It then accesses the "/etc/ld.so.cache" file and loads the "libc.so.6" library.
 The program creates two directories, "welcome_dir_1" and "welcome_dir_1/welcome_dir_2", using the mkdir system call.
 It then creates a new file called "welcome_file.txt" inside "welcome_dir_1 /welcome_dir_2" using the open and write
 system calls. The program reads the system time and writes "welcome to OS-2023" to the standard output.
 Finally, it removes the "welcome_file.txt" file and the "welcome_dir_1"
 and "welcome_dir_1/welcome_dir_2" directories using the unlink and rmdir system calls.


execve : Executes the program.

brk(NULL) : Returns the current program break

access :  Checks whether the file can be accessed for reading

openat : Opens the file for reading.

fstat : Obtains information about the file descriptor returned by the previous openat() call .

mmap : Maps the entire contents of the file into the process's virtual memory space, allowing it to be accessed directly from memory. 

close : Closes the file descriptor returned by the previous openat() call.

read : (...,N) Reads up to N bytes of data from the file descriptor returned by the previous openat() call.

mprotect : Changes the protection level of the given memory region (specified by the address and size) to prevent any access to it.


