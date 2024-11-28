#ifndef  DEVICE_H
#define  DEVICE_H

#include "fila.h"

typedef struct Device {
    int attendanceTime;
    Fila *queue;
    Process *process;
} Device;

Device createDevice(int attendanceTime);

#endif