#ifndef  FILA_H
#define  FILA_H

#include "process.h"

typedef struct fila Fila;
Fila* cria (void);
void insere (Fila* f, Process *process);
Process* retira (Fila* f);
int vazia (Fila* f);
void libera (Fila* f);
void exibe(Fila* f);
int tamanho(Fila* f);
int possui(Fila *f, Process *process);
#endif