#pragma once

#include <EEPROM.h>

#include "Reader.hpp"
#include "Writer.hpp"

class RepositoryReader final: public Reader {
public:    
    void begin();
    void commit();
    void end();

    uint8_t readNext();
private:
    int index;    
};

class RepositoryWriter final: public Writer {
public:    
    void begin();
    void commit();
    void end();

    void writeNext(uint8_t value);
private:
    int index;    
};
