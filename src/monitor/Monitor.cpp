
#include "Monitor.hpp"

Monitor::Monitor() : buffer(1024) {
}

void Monitor::write(Stream* target) {
    int available = buffer.available();

    uint8_t data[available];
    buffer.readBytes(data, sizeof(data));
    target->write(data, sizeof(data));
    target->flush();
}

size_t Monitor::write(uint8_t data) {
    Serial.write(data);    
    return buffer.write(data);
}
