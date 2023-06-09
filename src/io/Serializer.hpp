#pragma once

#include "Reader.hpp"
#include "Writer.hpp"
#include "automation/Z21Config.hpp"
#include "automation/AutomationData.hpp"

constexpr u_int8_t DATA_VERSION = 1;

class Serializer {
public:
    void write(const Z21Config* config, const AutomationData* data, Writer* writer);
    void read(Z21Config* config, AutomationData* data, Reader* reader);

 private:
    // Writing
    void writeConfig(const Z21Config* config, Writer* writer);
    void writeData(const AutomationData* data, Writer* writer);
    void writeScript(const Script* data, Writer* writer);
    void writeSection(const Section* data, Writer* writer);
    void writeAction(const Action* data, Writer* writer);
    void writeCondition(const Condition* data, Writer* writer);

    // Reading
    void readConfig(Z21Config* config, Reader* reader);   
    void readData(AutomationData* data, Reader* reader);
    void readScript(Script* data, Reader* reader);
    void readSection(Section* data, Reader* reader);
    void readAction(Action* data, Reader* reader);
    void readCondition(Condition* data, Reader* reader);
};

