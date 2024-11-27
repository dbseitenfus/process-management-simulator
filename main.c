//
//  main.c
//  process-management-simulator
//
//  Created by Daniel Seitenfus on 11/11/24.
//

#include <stdio.h>
#include <stdlib.h>
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

    int pidCounter = 0;
    Process processes[nProc];
    for(int i=0; i<nProc; i++) {
        fgets(line, sizeof(line), file);
        int tInicio = line[0] - '0';
        Process process = createProcess(pidCounter, tInicio);
        processes[i] = process;
        pidCounter++;
        insere(readyQueue, &processes[i]);
    }

    for(int i=0; i<nProc; i++) {
        Process process = processes[i];
        printf("\npid: %d, tInicio: %d", process.pid, process.tInicio);
    }

    exibe(readyQueue);

}
