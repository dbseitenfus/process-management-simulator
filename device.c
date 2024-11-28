#include "device.h"
#include <stdlib.h>

Device createDevice(int attendanceTime) {
    Device device;
    device.attendanceTime = attendanceTime;
    device.queue = cria();
    device.process = NULL;
    return device;
}