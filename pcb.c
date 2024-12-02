#include "pcb.h"
#include <stdlib.h>

PCB createProcess(int pid, int tInicio, int cyclesLength, int nDisp) {
    PCB process;
    process.pid = pid;
    process.tInicio = tInicio; 
    process.state = -1;
    process.pc = 0;
    process.cyclesLength = cyclesLength;
    process.tDevice = 0;

    process.deviceTime = (int*) malloc(nDisp * sizeof(int));
    for(int i=0; i<nDisp; i++) {
        process.deviceTime[i] = 0;
    }
    process.waitingTime = 0;
    process.tEnd = 0;
    return process;
}

void printState(State state) {
    if (state == NEW_READY) {
        printf("new_ready");
    } else if (state == READY) {
        printf("ready");
    } else if (state == RUNNING) {
        printf("running");
    } else if (state == BLOCKED) {
        printf("blocked");
    } else if (state == TERMINATED) {
        printf("terminated");
    } else {
        printf("--");
    }
}