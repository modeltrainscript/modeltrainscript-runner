
#include "Monitor.hpp"

void Monitor::write(Stream* target) {
    while(buffer.available()) {
        target->write(buffer.read());
    }
}

size_t Monitor::write(uint8_t data) {
    Serial.write(data);    
    return buffer.write(data);
}
