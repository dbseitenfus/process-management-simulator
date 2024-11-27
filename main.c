//
//  main.c
//  process-management-simulator
//
//  Created by Daniel Seitenfus on 11/11/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "fila.h"
#include "process.h"
#include "device.h"

void init(void);

int main(int argc, const char * argv[]) {
    init();
    return 0;
}

FILE* openFile(const char *filename) {
    FILE *fptr = fopen(filename, "r");
    
    if(fptr == NULL) {
      printf("Not able to open the file.");
        return NULL;
    }

    return fptr;
}

bool isSimulationFinished(Fila *terminatedQueue, int nProc) {
    if(tamanho(terminatedQueue) == nProc) {
        return true;
    }
    return false;
}

void printOutput(int tCPU) {
    printf("<%d>", tCPU);
}

void init(void) {
    FILE *file = openFile("input.txt");
    char line[100];
    fgets(line, sizeof(line), file);
    int nProc = line[0] - '0';
    int nDisp = line[2] - '0';
    printf("nProc: %d\n", nProc);
    printf("nDisp: %d\n", nDisp);


    struct Device devices[nDisp];
    int deviceTimeIndex = 4;
    for(int i=0; i<nDisp; i++) {
        struct Device device;
        device.attendanceTime = line[deviceTimeIndex] - '0';
        deviceTimeIndex += 2;
        devices[i] = device;
    }

    for(int i=0; i<nDisp; i++) {
        printf("\ndevice %d: tempo %d", i, devices[i].attendanceTime);
    }

    Fila *readyQueue = cria();
    Fila *terminatedQueue = cria();

    int pidCounter = 0;
    Process processesTable[nProc];
    for(int i=0; i<nProc; i++) {
        fgets(line, sizeof(line), file);
        int tInicio = line[0] - '0';
        int programLength = strlen(line)/2-1;
        printf("\nprogramLength: %d", programLength);
        // for(int i=2; i<strlen(line); i+=2) {
        //     printf(" t: %d,", line[i] - '0');
        // }
        Process process = createProcess(pidCounter, tInicio);
        process.program = (int*) malloc(programLength);
        processesTable[i] = process;
        pidCounter++;
        insere(readyQueue, &processesTable[i]);

        printf("\nprogram: ");
        for(int i=0; i<sizeof(process.program); i++){
            printf(" t: %d, ", process.program[i]);
        }
    }

    for(int i=0; i<nProc; i++) {
        Process process = processesTable[i];
        printf("\npid: %d, tInicio: %d", process.pid, process.tInicio);
    }

    int tCPU = 0;

    // while(!isSimulationFinished(terminatedQueue, nProc)) {

    // }

}
