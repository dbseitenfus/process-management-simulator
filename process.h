
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
    int pc;
    int programLength;
    int *program;
    State state;

    int tDevice;

    int *deviceTime;
    int waitingTime;
    int tEnd;
} Process;

Process createProcess(int pid, int tInicio, int programLength, int nDisp);
void printState(State state);

#endif