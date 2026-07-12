# cpp-custom-shell
# C++ Custom Command Line Shell

A lightweight UNIX command-line shell implemented in C++. This project demonstrates fundamental Operating System concepts, specifically process lifecycle management and system call interfacing.

## Core Features
* **Process Execution:** Spawns child processes to execute standard external Linux commands (e.g., `ls`, `grep`, `pwd`) using POSIX system calls.
* **Environment Modification:** Implements manual built-in commands like `cd` and `exit` that directly manipulate the parent shell's environment.
* **I/O Redirection:** Supports standard output redirection (`>`) to seamlessly write command outputs to text files instead of the terminal.
* **Input Parsing:** Efficiently tokenizes string inputs to separate execution commands from their standard arguments.

## Under the Hood (System Calls Used)
This shell bypasses high-level standard libraries to interact directly with the Linux kernel using:
* `fork()`: To duplicate the shell process and allocate memory for a child worker.
* `execvp()`: To replace the child process's memory space with the target executable.
* `waitpid()`: To suspend the parent shell until the child terminates, preventing zombie processes.
* `dup2()` and `open()`: To manipulate file descriptor tables and hijack the standard output stream for redirection.

## How to Compile and Run
This project is designed for Linux/UNIX environments. 

1. Compile the code using g++:
   ```bash
   g++ -O3 CustomShell.cpp -o myshell
