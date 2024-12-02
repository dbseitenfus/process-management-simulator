#include <stdlib.h>
#include <stdbool.h>
#include "fila.h"

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