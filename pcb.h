typedef enum State {
    NEW_READY,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} State;

typedef struct PCB {
    int pid;
    int tInicio;
    int pc;
    int cyclesLength;
    int *cycles;
    State state;
    int tDevice;
    int *deviceTime;
    int waitingTime;
    int tEnd;
} PCB;

PCB createProcess(int pid, int tInicio, int cyclesLength, int nDisp);
void printState(State state);