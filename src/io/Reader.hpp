#pragma once

#include <Arduino.h>
#include <Stream.h>

class Reader {
public:
    virtual uint8_t readNext() = 0;
};

class StreamReader: public Reader {
public:
    uint16_t bytesRead = 0;
    Stream* stream;
    StreamReader(Stream* stream);
    uint8_t readNext();
};