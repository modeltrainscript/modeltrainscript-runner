#pragma once

#include <Arduino.h>
#include <Stream.h>

class Writer {
public:
    virtual void writeNext(uint8_t value) = 0;
};

class StreamWriter: public Writer {
public:
    StreamWriter(Stream* stream);
    void writeNext(uint8_t value);
private:
    Stream* stream;
};