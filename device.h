#include "fila.h"

typedef struct Device {
    int attendanceTime;
    Fila *queue;
} Device;

Device createDevice(int attendanceTime);