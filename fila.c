#include <stdio.h>
#include <stdlib.h>
#include "fila.h"
#include "process.h"

// Estrutura para um nó da fila
typedef struct no {
    Process *process;
    struct no* prox;
} No;

// Estrutura da fila
typedef struct fila {
    No* inicio;
    No* fim;
} Fila;

// Cria uma fila vazia
Fila* cria(void) {
    Fila* f = (Fila*)malloc(sizeof(Fila));
    f->inicio = NULL;
    f->fim = NULL;
    return f;
}

// Insere um valor na fila
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
        f->inicio = novo; // Se a fila estava vazia, o início também aponta para o novo nó
    }
    f->fim = novo;
}

// Retira um valor da fila
Process* retira(Fila* f) {
    if (vazia(f)) {
        printf("Erro: Fila vazia\n");
        exit(1);
    }
    No* t = f->inicio;
    Process *process = t->process;
    f->inicio = t->prox;
    if (f->inicio == NULL) { // Se a fila ficou vazia, atualiza o fim
        f->fim = NULL;
    }
    free(t);
    return process;
}

// Verifica se a fila está vazia
int vazia(Fila* f) {
    return (f->inicio == NULL);
}

// Libera a memória ocupada pela fila
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
        printf("A fila está vazia.\n");
        return;
    }

    No* atual = f->inicio;
    printf("\nConteúdo da fila:\n");
    while (atual != NULL) {
        Process* process = atual->process;
        printf("PID: %d, tInicio: %d, tCPU: %d, Estado: ",
               process->pid, process->tInicio, process->tCPU);
        printState(process->state);
        printf("\n");
        atual = atual->prox;
    }
}
