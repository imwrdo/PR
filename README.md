# Subject Title: Distributed Processing

## About

Welcome to the Distributed Processing University Course Repository!
This repository contains completed labs for the Distributed Processing course at my Univeresity. The labs are designed to cover various topics related to distributed systems, 
parallel computing, and distributed processing techniques.



## Labs Description
1. Dynamic Data Structures
  - Folder: Lab1
  - Status: Completed and Published
2. Static and dynamic libraries in Linux
  - Folder: Lab2
  - Status: Completed and Published
  - Description: I was to build static (archive) and dynamic (shared bibl.) libraries offering min. 4 arbitrary calculation functions, . Each function has a dedicated separate pair of *.c /*.h   files. I wrote code that loads and checks the operation of both libraries (in shell script files or makefiles).
3. Dynamic Data Structures
  - Folder: Lab3
  - Status: Completed and Published
  - Description: script.sh: a script that allows you to run min. 4 commands e.g. wget , ping , sleep etc. The script closes only when all child processes have finished.
The script allows the flexibility to add (any number!) of new commands to run.
main.c: the same as in the script, but this time implemented in C code.
The script and .c program have menu
4. Process and Thread Management
  - Folder: Lab4
  - Status: Completed and Published
  - Description: In this lab, we learned about managing processes and threads with two console programs: ProgramGlowny.exe and ProgramPodrzedny.exe.
ProgramGlowny.exe allows users to control processes. It can create new processes with adjustable priorities, stop specific processes, list running processes, and close the program, ending all associated processes.
ProgramPodrzedny.exe handles thread management within its own process. Users can create threads, adjust priorities, delete threads, and list active threads. Additionally, the program can terminate the entire process, stopping all threads.
This lab provided practical experience in managing processes and threads within a console environment.
5.  Inter-process communication in Windows
  - Folder: Lab5
  - Status: Completed and Published
  - Description: The given folder contains two independent window programs:for tracking basic messages (Windows message loop);for communication via the Windows clipboard in its own format.
6. Windows sockets interface
  - Folder: Lab6
  - Status: Completed and Published
  - Description:  This lab requires the development of two applications, server, and client with:
     - Support for concurrent handling of multiple clients through synchronous sockets + threads or potentially asynchronous sockets
     - The client triggers time-consuming calculations on the server (minimum 30 sec., like prime number search) or forces server closure based on user choice
     - The server uses a shared file for all clients, storing partial results during calculations along with client identification, and ensures proper synchronization of access (critical section)
     - Bidirectional communication between client and server is mandatory.


