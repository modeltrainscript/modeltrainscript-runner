#include <Arduino.h>

#include "Serializer.hpp"

// Writing
void Serializer::write(const Config* config, const AutomationData* data, Writer* writer) {
    writer->writeNext(1);
    writer->writeNext(DATA_VERSION);

    writeConfig(config, writer);

    writeData(data, writer);
}

void Serializer::writeConfig(const Config* config, Writer* writer) {    
    writer->writeNext(config->ipAddress[0]);
    writer->writeNext(config->ipAddress[1]);
    writer->writeNext(config->ipAddress[2]);
    writer->writeNext(config->ipAddress[3]);

    writer->writeNext(config->port >> 8);
    writer->writeNext(config->port);
}

void Serializer::writeData(const AutomationData* data, Writer* writer) {
    writer->writeNext(data->numScripts);
    writer->writeNext(0); // RESERVED

    for (int i=0; i < data->numScripts; ++i) {
        writeScript(&(data->scripts[i]), writer);
    }
}

void Serializer::writeScript(const Script* script, Writer* writer) {
    writer->writeNext(script->numSections);
    writer->writeNext(0); // RESERVED

    writeCondition(&(script->event), writer);

    for (int i=0; i < script->numSections; ++i) {
        writeSection(&(script->sections[i]), writer);
    }
}

void Serializer::writeSection(const Section* section, Writer* writer) {
    writer->writeNext((section->conditional ? 1 << 6 : 0) + (section->repeatEnabled ? 1 << 5 : 0) + (section->numActions &0xFF));
    writer->writeNext(0); // RESERVED

    if (section->conditional) {
        writeCondition(&(section->condition), writer);
    }

    if (section->repeatEnabled) {
        writeCondition(&(section->repeatCondition), writer);
    }

    for( int i=0; i < section->numActions; ++i) {
        writeAction(&(section->actions[i]), writer);
    }
}

void Serializer::writeAction(const Action* action, Writer* writer) {
    writer->writeNext(static_cast<uint8_t>(action->type));
    writer->writeNext(0); // RESERVED
    writer->writeNext(action->input1 >> 8);
    writer->writeNext(action->input1);
    writer->writeNext(action->input2 >> 8);
    writer->writeNext(action->input2);
    writer->writeNext(0); // RESERVED
    writer->writeNext(0); // RESERVED
}

void Serializer::writeCondition(const Condition* condition, Writer* writer) {
    writer->writeNext((condition->active ? 1 << 5 : 0) + static_cast<uint8_t>(condition->type));
    writer->writeNext(condition->address >> 8);
    writer->writeNext(condition->address);
    writer->writeNext(condition->input);
    writer->writeNext(0); // RESERVED
    writer->writeNext(0); // RESERVED
}

// Reading
void Serializer::read(Config* config, AutomationData* data, Reader* reader) {    
    int value = reader->readNext();
    reader->readNext();

    if (value == 1) { // Has data
        readConfig(config, reader);

        readData(data, reader);
    }
}

void Serializer::readConfig(Config* config, Reader* reader) {
    config->ipAddress[0] = reader->readNext();
    config->ipAddress[1] = reader->readNext();
    config->ipAddress[2] = reader->readNext();
    config->ipAddress[3] = reader->readNext();

    config->port = (reader->readNext() << 8) + reader->readNext();
}

void Serializer::readData(AutomationData* data, Reader* reader) {
    data->numScripts = reader->readNext();
    reader->readNext(); // RESERVED

    for (int i=0; i < data->numScripts; ++i) {        
        readScript(&(data->scripts[i]), reader);
    }
}

void Serializer::readScript(Script* script, Reader* reader) {
    script->numSections = reader->readNext();
    reader->readNext(); // RESEVERED

    readCondition(&(script->event), reader);

    for (int i=0; i < script->numSections; ++i) {        
        readSection(&(script->sections[i]), reader);
    }
}

void Serializer::readSection(Section* section, Reader* reader) {
    int first = reader->readNext();
    section->conditional = first & 0x40;
    section->repeatEnabled = first & 0x20;
    section->numActions = first & 0x0F;
    
    reader->readNext(); // RESERVED

    if (section->conditional) {
        readCondition(&(section->condition), reader);
    }

    if (section->repeatEnabled) {
        readCondition(&(section->repeatCondition), reader);
    }

    for (int i=0; i < section->numActions; ++i) {
        readAction(&(section->actions[i]), reader);        
    }
}

void Serializer::readAction(Action* action, Reader* reader) {
    uint8_t type = reader->readNext();

    action->type = static_cast<ActionType>(type);

    reader->readNext(); // RESERVED    
    action->input1 = (reader->readNext() << 8) + reader->readNext();
    action->input2 = (reader->readNext() << 8) + reader->readNext();
    reader->readNext(); // RESERVED    
    reader->readNext(); // RESERVED    
}

void Serializer::readCondition(Condition* condition, Reader* reader) {
    int first = reader->readNext();
    condition->active = first & 0x20;

    condition->type = static_cast<ConditionType>(first & 0x0F);

    condition->address = (reader->readNext() << 8) + reader->readNext();

    condition->input = reader->readNext();
    reader->readNext(); // RESERVED    
    reader->readNext(); // RESERVED        
}
