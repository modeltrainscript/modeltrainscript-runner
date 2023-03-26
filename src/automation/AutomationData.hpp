#pragma once

#include "Context.hpp"
#include "Script.hpp"

class AutomationData {
public:
    int numScripts;
    Script scripts[SR_MAX_SCRIPTS];

    void setup(SetupContext* context);
    void loop(AutomationContext* context);
    void accessoryUpdated(unsigned int address, uint8_t accessoryState);
    void onRBusDataChanged(uint8_t groupId, uint8_t* data, uint16_t length);
    void onCanDetector(uint16_t nld, uint16_t addr, uint8_t port, uint8_t type, uint16_t value1, uint16_t value2);
    void onGPIOPinChange(uint8_t pin, bool active);
};