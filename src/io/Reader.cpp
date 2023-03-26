#include "Reader.hpp"

// StreamReader
StreamReader::StreamReader(Stream* stream) {
    this->stream = stream;
}

uint8_t StreamReader::readNext() {
    bytesRead++;
    return stream->read();
}