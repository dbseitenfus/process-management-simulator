#include <stdio.h>
#include "process.h"

Process createProcess(int pid, int tInicio) {
    Process process;
    process.pid = pid;
    process.tInicio = tInicio; 
    process.state = NEW_READY;
    return process;
}

void printState(State state) {
    if (state == NEW_READY) {
        printf("NEW_READY");
    } else if (state == RUNNING) {
        printf("RUNNING");
    } else if (state == BLOCKED) {
        printf("BLOCKED");
    } else if (state == TERMINATED) {
        printf("TERMINATED");
    }
}