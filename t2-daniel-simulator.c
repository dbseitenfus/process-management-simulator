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

typedef struct fila Fila;
Fila* criar (void);
void insere (Fila* f, PCB *process);
PCB* retira (Fila* f);
int vazia (Fila* f);
int possui(Fila *f, PCB *process);

typedef struct no {
    PCB *process;
    struct no* prox;
} No;

typedef struct fila {
    No* inicio;
    No* fim;
} Fila;

Fila* criar(void) {
    Fila* f = (Fila*)malloc(sizeof(Fila));
    f->inicio = NULL;
    f->fim = NULL;
    return f;
}

void insere(Fila* f, PCB *process) {
    No* novo = (No*)malloc(sizeof(No));
    if (novo == NULL) {
        printf("Erro de alocação de memória\n");
        exit(1);
    }
    novo->process = process;
    novo->prox = NULL;
    if (f->fim != NULL) {
        f->fim->prox = novo;
    } else {
        f->inicio = novo;
    }
    f->fim = novo;
}

PCB* retira(Fila* f) {
    if (vazia(f)) {
        return NULL;
    }
    No* t = f->inicio;
    PCB *process = t->process;
    f->inicio = t->prox;
    if (f->inicio == NULL) { 
        f->fim = NULL;
    }
    free(t);
    return process;
}

int vazia(Fila* f) {
    return (f->inicio == NULL);
}

int possui(Fila *f, PCB *process) {
    if (vazia(f)) {
        return 0;
    }

    No* atual = f->inicio;
    while (atual != NULL) {
        PCB* process = atual->process;
        if(atual->process == process) {
            return true;
        }
        atual = atual->prox;
    }

    return false;
}

typedef struct Device {
    int attendanceTime;
    Fila *queue;
    PCB *process;
} Device;

Device createDevice(int attendanceTime);

Device createDevice(int attendanceTime) {
    Device device;
    device.attendanceTime = attendanceTime;
    device.queue = criar();
    device.process = NULL;
    return device;
}

void init(void);
FILE* openFile(const char *filename);
void checkNewProcesses(PCB *processesTable, int tCPU, int nProc, Fila *readyQueue);
bool isSimulationFinished(PCB *processesTable, int nProc);
PCB* scheduleShortTerm(Fila *readyQueue);
bool isProcessEnd(PCB *process);
void endProcess(PCB *process, int tCPU, Fila *terminatedQueue);
void run(PCB *processesTable, int nProc, Device *devicesTable, int nDisp, Fila *readyQueue, Fila *terminatedQueue);
void handleDevices(Device *devicesTable, int nDisp);
void printOutput(FILE *outputFile, int tCPU, PCB *processesTable, Device *devicesTable, int nProc, int nDisp);
void printReport(FILE *outputFile, PCB *processTable, int tCPU, int nProc, int nDisp, int cpuIdleTime);
void printStateToFile(FILE *outputFile, State state);

int main(int argc, const char * argv[]) {
    init();
    return 0;
}

FILE* openFile(const char *filename) {
    FILE *fptr = fopen(filename, "r");
    
    if(fptr == NULL) {
      printf("\nNão foi possível abrir o arquivo.");
        return NULL;
    }

    return fptr;
}

bool isSimulationFinished(PCB *processesTable, int nProc) {
    for(int i=0; i<nProc; i++) {
        PCB process = processesTable[i];
        if(process.state != TERMINATED) {
            return false;
        }
    }

    return true;
}

void checkNewProcesses(PCB *processesTable, int tCPU, int nProc, Fila *readyQueue) {
    for(int i=0; i<nProc; i++) {
        PCB *process = &processesTable[i];
        if(process->state == -1 && process->tInicio == tCPU) {
            process->state = NEW_READY;
        }
    }
}

PCB* scheduleShortTerm(Fila *readyQueue) {
    PCB *scheduledProcess = retira(readyQueue);
    if(scheduledProcess != NULL) scheduledProcess->state = RUNNING;
    return scheduledProcess;
}

bool isProcessEnd(PCB *process) {
    return process->pc >= process->cyclesLength-1;
}

void endProcess(PCB *process, int tCPU, Fila *terminatedQueue) {
    process->state = TERMINATED;
    process->tEnd = tCPU;
    insere(terminatedQueue, process);
}

void checkProcessEnd(PCB *processTable, int tCPU, int nProc, Fila *terminatedQueue) {
    for(int i=0; i<nProc; i++) {
        PCB *process = &processTable[i];
        if(isProcessEnd(process) && process->state != TERMINATED) { 
            endProcess(process, tCPU, terminatedQueue);
        }
    }
}

void insertProcessInDeviceQueue(PCB *process, Device *device) {
    insere(device->queue, process);
}

int scheduleLongTerm(PCB *processesTable, PCB *scheduledProcess, Device *devicesTable, int timeSlice, int nProc, int nDisp, int tCPU, Fila *readyQueue, Fila *terminatedQueue){
    int return_type = 0;
    for(int i=0; i<nProc; i++) {
        PCB *process = &processesTable[i];
        if(process->state == NEW_READY) {
            process->state = READY;
            insere(readyQueue, process);
        }
    }

    for(int i=0; i<nDisp; i++) {
        Device *device = &devicesTable[i];
        PCB *process = device->process;
        if(process != NULL) {
            if(process->tDevice <= 0) {
                if(!isProcessEnd(process) && process->state != TERMINATED) { 
                    process->state = READY;
                    device->process->pc++;
                    insere(readyQueue, device->process);
                }
                device->process = NULL;
            }
        }
    }
    if(scheduledProcess != NULL) {
        if(scheduledProcess->cycles[scheduledProcess->pc] == 0) { 
            if(!isProcessEnd(scheduledProcess)) {
                scheduledProcess->pc++;
                int deviceId = scheduledProcess->cycles[scheduledProcess->pc];
                Device *device = &devicesTable[deviceId-1];
                insertProcessInDeviceQueue(scheduledProcess, device);
                scheduledProcess->state = BLOCKED;
            } else {
                endProcess(scheduledProcess, tCPU, terminatedQueue);
            }

            return_type = 1;
        }
    }

    checkNewProcesses(processesTable, tCPU, nProc, readyQueue);

    return return_type;
}


void calculateStatistics(PCB *processTable, int nProc) {
    for(int i=0; i<nProc; i++) {
        PCB *process = &processTable[i];
        if(process->state == READY) {
            process->waitingTime++;
        }else if(process->state == BLOCKED) {
            process->deviceTime[process->cycles[process->pc]-1]++; 
        }
    }
}

void preemptProcess(PCB *process, int tCPU, Fila *readyQueue, Fila *terminatedQueue) {
    process->state = READY;
    insere(readyQueue, process);
}

void readDevices(Device *devicesTable, int nDisp, char line[100]) {
    int deviceTimeIndex = 4;
    for(int i=0; i<nDisp; i++) {
        int attendanceTime = line[deviceTimeIndex] - '0';
        devicesTable[i] = createDevice(attendanceTime);
        deviceTimeIndex += 2;
    }
}

void readProcesses(FILE *file, PCB *processesTable, int nProc, int nDisp) {
    char line[100];
    int pidCounter = 1;
    for(int i=0; i<nProc; i++) {
        fgets(line, sizeof(line), file);
        int tInicio = line[0] - '0';
        int cyclesLength = strlen(line)/2-1;        
        PCB process = createProcess(pidCounter, tInicio, cyclesLength, nDisp);
        process.cycles = (int*) malloc(cyclesLength * sizeof(int));
        int cyclesIndex = 0;
        for(int i=2; i<strlen(line); i+=2) {
            process.cycles[cyclesIndex] =  line[i] - '0';
            cyclesIndex++;
        }
        processesTable[i] = process;
        pidCounter++;
    }
}

void init(void) {
    FILE *file = openFile("input.txt");
    char line[100];
    fgets(line, sizeof(line), file);
    int nProc = line[0] - '0';
    int nDisp = line[2] - '0';

    Device devicesTable[nDisp];
    readDevices(devicesTable, nDisp, line);

    Fila *readyQueue = criar();
    Fila *terminatedQueue = criar();

    PCB processesTable[nProc];
    readProcesses(file, processesTable, nProc, nDisp);

    run(processesTable, nProc, devicesTable, nDisp, readyQueue, terminatedQueue);
}

void run(PCB *processesTable, int nProc, Device *devicesTable, int nDisp, Fila *readyQueue, Fila *terminatedQueue) {
    FILE *outputFile = fopen("output.txt", "w");
    int tCPU = 0;
    int timeSlice = 0;
    int cpuIdleTime = 0;
    PCB *scheduledProcess = NULL;

    while(!isSimulationFinished(processesTable, nProc)) {

        checkProcessEnd(processesTable, tCPU, nProc, terminatedQueue);
        int result = scheduleLongTerm(processesTable, scheduledProcess, devicesTable, timeSlice, nProc, nDisp, tCPU, readyQueue, terminatedQueue);
        if(result == 1) {
            scheduledProcess = NULL;
            timeSlice = 0;
        }

        if(scheduledProcess != NULL) {
            if(timeSlice >= 4) { 
                preemptProcess(scheduledProcess, tCPU, readyQueue, terminatedQueue);
                timeSlice = 0;
                scheduledProcess = NULL;
            } 
        }

        if(scheduledProcess == NULL || scheduledProcess->state != RUNNING) { 
            scheduledProcess = scheduleShortTerm(readyQueue); 
            timeSlice = 0;
        }

        if(scheduledProcess != NULL) { 
            scheduledProcess->cycles[scheduledProcess->pc]--;
            timeSlice++;
        } else {
            cpuIdleTime++;
        }

        handleDevices(devicesTable, nDisp);
        
        calculateStatistics(processesTable, nProc);
        printOutput(outputFile, tCPU, processesTable, devicesTable, nProc, nDisp);

        tCPU++;
    }

    printReport(outputFile, processesTable, tCPU, nProc, nDisp, cpuIdleTime);
}

void handleDevices(Device *devicesTable, int nDisp) {
    for(int i=0; i<nDisp; i++) {
        Device *device = &devicesTable[i];
        
        if(device->process == NULL && !vazia(device->queue)) {
            device->process = retira(device->queue);
            device->process->tDevice = device->attendanceTime;
        }
        
        if(device->process != NULL){
            device->process->tDevice--;
        }
    }
}

void printOutput(FILE *outputFile, int tCPU, PCB *processesTable, Device *devicesTable, int nProc, int nDisp) {
    if(tCPU == 0) {
        fprintf(outputFile, "<%02d> |", tCPU);
    } else {
        fprintf(outputFile, "\n<%02d> |", tCPU);
    }
    
    for(int i=0; i<nProc; i++) {
        PCB *process = &processesTable[i];
        fprintf(outputFile, " P%02d ", process->pid);
        fprintf(outputFile, "state: ");
        printStateToFile(outputFile, process->state);
        if(process->state == BLOCKED) {
            int deviceId = process->cycles[process->pc];
            if(possui(devicesTable[deviceId-1].queue, process)){
                fprintf(outputFile, " queue");
            }
            fprintf(outputFile, " d%d", deviceId);
        }
        fprintf(outputFile, " |");
    }
}

void printReport(FILE *outputFile, PCB *processTable, int tCPU, int nProc, int nDisp, int cpuIdleTime) {
    for(int i=0; i<nProc; i++) {
        PCB process = processTable[i];
        fprintf(outputFile, "\n| P%02d ", process.pid);

        for(int j=0; j<nDisp; j++) {
            fprintf(outputFile, " device time d%d: %d,", j+1, process.deviceTime[j]);
        }

        fprintf(outputFile, " waiting time: %d, ", process.waitingTime);
        fprintf(outputFile, "throughput: %d", process.tEnd - process.tInicio);
    }

    fprintf(outputFile, "\nCPU idle time: %d", cpuIdleTime);
}

void printStateToFile(FILE *outputFile, State state) {
    if (state == NEW_READY) {
        fprintf(outputFile, "new_ready");
    } else if (state == READY) {
        fprintf(outputFile, "ready");
    } else if (state == RUNNING) {
        fprintf(outputFile, "running");
    } else if (state == BLOCKED) {
        fprintf(outputFile, "blocked");
    } else if (state == TERMINATED) {
        fprintf(outputFile, "terminated");
    } else {
        fprintf(outputFile, "--");
    }
}
