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

typedef struct Process {
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
} Process;

Process createProcess(int pid, int tInicio, int cyclesLength, int nDisp);
void printState(State state);

Process createProcess(int pid, int tInicio, int cyclesLength, int nDisp) {
    Process process;
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
Fila* cria (void);
void insere (Fila* f, Process *process);
Process* retira (Fila* f);
int vazia (Fila* f);
void libera (Fila* f);
void exibe(Fila* f);
int tamanho(Fila* f);
int possui(Fila *f, Process *process);


typedef struct no {
    Process *process;
    struct no* prox;
} No;

typedef struct fila {
    No* inicio;
    No* fim;
} Fila;

Fila* cria(void) {
    Fila* f = (Fila*)malloc(sizeof(Fila));
    f->inicio = NULL;
    f->fim = NULL;
    return f;
}

void insere(Fila* f, Process *process) {
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

Process* retira(Fila* f) {
    if (vazia(f)) {
        return NULL;
    }
    No* t = f->inicio;
    Process *process = t->process;
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

void libera(Fila* f) {
    No* atual = f->inicio;
    while (atual != NULL) {
        No* temp = atual->prox;
        free(atual);
        atual = temp;
    }
    free(f);
}

void exibe(Fila* f) {
    if (vazia(f)) {
        return;
    }

    No* atual = f->inicio;
    while (atual != NULL) {
        Process* process = atual->process;
        printf("PID: %d, tInicio: %d, Estado: ",
               process->pid, process->tInicio);
        printState(process->state);
        printf("\n");
        atual = atual->prox;
    }
}

int tamanho(Fila* f) {
    int count = 0;
    No* atual = f->inicio;
    
    while (atual != NULL) {
        count++;
        atual = atual->prox;
    }
    
    return count;
}

int possui(Fila *f, Process *process) {
    if (vazia(f)) {
        return 0;
    }

    No* atual = f->inicio;
    while (atual != NULL) {
        Process* process = atual->process;
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
    Process *process;
} Device;

Device createDevice(int attendanceTime);

Device createDevice(int attendanceTime) {
    Device device;
    device.attendanceTime = attendanceTime;
    device.queue = cria();
    device.process = NULL;
    return device;
}


void init(void);
FILE* openFile(const char *filename);
void checkNewProcesses(Process *processesTable, int tCPU, int nProc, Fila *readyQueue);
bool isSimulationFinished(Process *processesTable, int nProc);
void printOutput(int tCPU, Process *processesTable, Device *devices, int nProc, int nDisp);
Process* scheduleShortTerm(Fila *readyQueue);
bool isProcessEnd(Process *process);
void endProcess(Process *process, int tCPU, Fila *terminatedQueue);

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

bool isSimulationFinished(Process *processesTable, int nProc) {
    for(int i=0; i<nProc; i++) {
        Process process = processesTable[i];
        if(process.state != TERMINATED) {
            return false;
        }
    }

    return true;
}

void printOutput(int tCPU, Process *processesTable, Device *devices, int nProc, int nDisp) {
    printf("\n<%02d> |", tCPU);
    for(int i=0; i<nProc; i++) {
        Process *process = &processesTable[i];
        printf(" P%02d ", process->pid);
        printf("state: ");
        printState(process->state);
        if(process->state == BLOCKED) {
            int deviceId = process->cycles[process->pc];
            if(possui(devices[deviceId-1].queue, process)){
                printf(" queue");
            }
            printf(" d%d", deviceId);
        }
        printf(" |");
    }
}

void checkNewProcesses(Process *processesTable, int tCPU, int nProc, Fila *readyQueue) {
    for(int i=0; i<nProc; i++) {
        Process *process = &processesTable[i];
        if(process->state == -1) {
            if(process->tInicio == tCPU) {
                process->state = NEW_READY;
            }
        }
    }
}

Process* scheduleShortTerm(Fila *readyQueue) {
    Process *scheduledProcess = retira(readyQueue);
    if(scheduledProcess != NULL) scheduledProcess->state = RUNNING;
    return scheduledProcess;
}

bool isProcessEnd(Process *process) {
    return process->pc >= process->cyclesLength-1;
}

void endProcess(Process *process, int tCPU, Fila *terminatedQueue) {
    process->state = TERMINATED;
    process->tEnd = tCPU;
    insere(terminatedQueue, process);
}

void checkProcessEnd(Process *processTable, int tCPU, int nProc, Fila *terminatedQueue) {
    for(int i=0; i<nProc; i++) {
        Process *process = &processTable[i];
        if(isProcessEnd(process) && process->state != TERMINATED) { 
            endProcess(process, tCPU, terminatedQueue);
        }
    }
}

void insertProcessInDeviceQueue(Process *process, Device *device) {
    insere(device->queue, process);
}

void scheduleLongTerm(Process *processesTable, Process *scheduledProcess, Device *devices, int *timeSlice, int nProc, int nDisp, int tCPU, Fila *readyQueue, Fila *terminatedQueue){
    for(int i=0; i<nProc; i++) {
        Process *process = &processesTable[i];
        if(process->state == NEW_READY) {
            process->state = READY;
            insere(readyQueue, process);
        }
    }

    for(int i=0; i<nDisp; i++) {
        Device *device = &devices[i];
        Process *process = device->process;
        if(process != NULL) {
            if(process->tDevice == 0) { //se chegou no tempo de atendimento
                if(!isProcessEnd(process) && process->state != TERMINATED) { //acho que isso não pode ocorrer aqui.
                    process->state = READY;
                    device->process->pc++;
                    insere(readyQueue, device->process);
                }
                device->process = NULL;
            }
        }
    }
    if(scheduledProcess != NULL) {
        if(scheduledProcess->cycles[scheduledProcess->pc] == 0) { //verifica se o processo usou o tCPU que ele queria
            //entrega ao device
            if(!isProcessEnd(scheduledProcess)) {
                scheduledProcess->pc++;
                int deviceId = scheduledProcess->cycles[scheduledProcess->pc];
                Device *device = &devices[deviceId-1];
                insertProcessInDeviceQueue(scheduledProcess, device);

                scheduledProcess->state = BLOCKED; //?
            } else {
                endProcess(scheduledProcess, tCPU, terminatedQueue);
            }

            scheduledProcess = NULL;
            timeSlice = 0;

        }
    }
    

    checkNewProcesses(processesTable, tCPU, nProc, readyQueue);
}

void printReport(Process *processTable, int tCPU, int nProc, int nDisp, int cpuIdleTime) {
    for(int i=0; i<nProc; i++) {
        Process process = processTable[i];
        printf("\n| P%02d ", process.pid);

        for(int j=0; j<nDisp; j++) {
            printf(" device time d%d: %d,", j+1, process.deviceTime[j]);
        }

        printf(" waiting time: %d, ", process.waitingTime);
        printf("throughput: %d", process.tEnd - process.tInicio);

    }

    printf("\nCPU idle time: %d", cpuIdleTime);
}

void calculateStatistics(Process *processTable, int nProc) {
    for(int i=0; i<nProc; i++) {
        Process *process = &processTable[i];
        if(process->state == READY) {
            process->waitingTime++;
        }else if(process->state == BLOCKED) {
            process->deviceTime[process->cycles[process->pc]-1]++; 
        }
    }
}

void preemptProcess(Process *process, int tCPU, Fila *readyQueue, Fila *terminatedQueue) {
    process->state = READY;
    insere(readyQueue, process); //acho que isso não pode ocorrer aqui. o processo pode ter atingido a fatia de tempo e terminado. isso significa que ele não pode entrar novamente na fila de ready
}



void init(void) {
    FILE *file = openFile("input.txt");
    char line[100];
    fgets(line, sizeof(line), file);
    int nProc = line[0] - '0';
    int nDisp = line[2] - '0';

    Device devices[nDisp];
    int deviceTimeIndex = 4;
    for(int i=0; i<nDisp; i++) {
        int attendanceTime = line[deviceTimeIndex] - '0';
        devices[i] = createDevice(attendanceTime);
        deviceTimeIndex += 2;
    }

    Fila *readyQueue = cria();
    Fila *terminatedQueue = cria();

    int pidCounter = 1;
    Process processesTable[nProc];
    for(int i=0; i<nProc; i++) {
        fgets(line, sizeof(line), file);
        int tInicio = line[0] - '0';
        int cyclesLength = strlen(line)/2-1;        
        Process process = createProcess(pidCounter, tInicio, cyclesLength, nDisp);
        process.cycles = (int*) malloc(cyclesLength * sizeof(int));
        int cyclesIndex = 0;
        for(int i=2; i<strlen(line); i+=2) {
            process.cycles[cyclesIndex] =  line[i] - '0';
            cyclesIndex++;
        }
        processesTable[i] = process;
        pidCounter++;
    }

    printf("\n\n---------Início da simulação ---------\n");
    int tCPU = 0;
    int timeSlice = 0;
    int cpuIdleTime = 0;
    Process *scheduledProcess;
    while(!isSimulationFinished(processesTable, nProc)) {

      
        
        checkProcessEnd(processesTable, tCPU, nProc, terminatedQueue);
        scheduleLongTerm(processesTable, scheduledProcess, devices, &timeSlice, nProc, nDisp, tCPU, readyQueue, terminatedQueue);

        if(scheduledProcess != NULL) {
            if(timeSlice >= 4) { //verifica se ele já usou toda fatia dele
                preemptProcess(scheduledProcess, tCPU, readyQueue, terminatedQueue);
                timeSlice = 0;
                scheduledProcess = NULL;
            } 
        }

        if(scheduledProcess == NULL || scheduledProcess->state != RUNNING) { //nao tem processo e verifica se há processo para scalonar
            scheduledProcess = scheduleShortTerm(readyQueue); //TODO: Aqui verificar se o processo é diferente 
            timeSlice = 0;
        }

        if(scheduledProcess != NULL) { //tem processo para executar
            scheduledProcess->cycles[scheduledProcess->pc]--;
            timeSlice++;
        } else {
            cpuIdleTime++;
        }

        //ALTERAR: A inserção na fila ready se dá verificando os processos que se encontram em estado “new/ready”, nas filas de dispositivos e na CPU (nesta ordem).

        for(int i=0; i<nDisp; i++) {
            Device *device = &devices[i];
            
            if(device->process == NULL && !vazia(device->queue)) { // se não tem processo usando o device e tem algum na fila
                device->process = retira(device->queue);
                device->process->tDevice = device->attendanceTime;
            }
            
            if(device->process != NULL){
                device->process->tDevice--;
            }
        }
        calculateStatistics(processesTable, nProc);
        printOutput(tCPU, processesTable, devices, nProc, nDisp);

        tCPU++;
    }

    printReport(processesTable, tCPU, nProc, nDisp, cpuIdleTime);
}
