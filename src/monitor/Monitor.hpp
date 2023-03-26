#pragma once

#include <Arduino.h>
#include <LoopbackStream.h>

class Monitor: public Print {
public:
    void write(Stream* target);
private:
    LoopbackStream buffer;
    
    size_t write(uint8_t);
};