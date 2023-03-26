#include "Repository.hpp"

#define EEPROM_SIZE 512

// Reader
void RepositoryReader::begin() {
    EEPROM.begin(EEPROM_SIZE);
    index = 0;
}

void RepositoryReader::commit() {
    EEPROM.commit();
}

void RepositoryReader::end() {
    EEPROM.end();
    index = 0;
}

uint8_t RepositoryReader::readNext() {
    uint8_t result = EEPROM.read(index);
    index++;
    return result;
}

// Writer
void RepositoryWriter::begin() {
    EEPROM.begin(EEPROM_SIZE);
    index = 0;
}

void RepositoryWriter::commit() {
    EEPROM.commit();
}

void RepositoryWriter::end() {
    EEPROM.end();
    index = 0;
}

void RepositoryWriter::writeNext(uint8_t value) {
    EEPROM.write(index, value);
    index++;
}

