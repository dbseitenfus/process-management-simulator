
#ifndef  PROCESS_H
#define  PROCESS_H

typedef enum State {
    NEW_READY,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} State;

typedef struct Process {
    int pid;
    int tInicio;
    int tCPU;
    int pc;
    int *program;
    State state;
} Process;

Process createProcess(int pid, int tInicio);
void printState(State state);

#endif