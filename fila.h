#include "pcb.h"

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