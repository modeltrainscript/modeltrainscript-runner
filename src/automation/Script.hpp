#pragma once

#include "Context.hpp"
#include "Condition.hpp"
#include "Section.hpp"

constexpr int8_t SCRIPT_STATE_COMPLETED = -1;

class Script {
public:        
    Condition event;
    int numSections;    
    Section sections[SR_MAX_SCRIPT_SECTIONS];

    void setup(SetupContext* context);
    void loop(AutomationContext* context);    
    void accessoryUpdated(unsigned int address, uint8_t accessoryState);
    void onRBusDataChanged(uint8_t groupId, uint8_t* data, uint16_t length);
    void onCanDetector(uint16_t nld, uint16_t addr, uint8_t port, uint8_t type, uint16_t value1, uint16_t value2);
    void onGPIOPinChange(uint8_t pin, bool active);
private:
    int state = -1;

    void executeNext(AutomationContext* context);
};
