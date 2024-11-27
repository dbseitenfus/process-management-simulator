#include "device.h"

Device createDevice(int attendanceTime) {
    Device device;
    device.attendanceTime = attendanceTime;
    device.queue = cria();
}