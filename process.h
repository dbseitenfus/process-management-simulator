
#ifndef  PROCESS_H
#define  PROCESS_H

typedef enum State {
    NEW_READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} State;

typedef struct Process {
    int pid;
    int tInicio;
    enum State state;
    int tCPU;

    int PCB;
} Process;

Process createProcess(int pid, int tInicio);
void printState(State state);

#endif