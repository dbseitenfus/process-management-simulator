#include "fila.h"
#include "pcb.h"

typedef struct Device {
    int attendanceTime;
    Fila *queue;
    PCB *process;
} Device;

Device createDevice(int attendanceTime);