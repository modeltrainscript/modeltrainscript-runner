#include "Writer.hpp"

// StreamWriter
StreamWriter::StreamWriter(Stream* stream) {
    this->stream = stream;
}

void StreamWriter::writeNext(uint8_t value) {
    stream->write(value);
}
