#include <stdlib.h>
#include "device.h"

Device createDevice(int attendanceTime) {
    Device device;
    device.attendanceTime = attendanceTime;
    device.queue = criar();
    device.process = NULL;
    return device;
}