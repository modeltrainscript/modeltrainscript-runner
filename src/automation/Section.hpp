#pragma once

#include "Context.hpp"
#include "Condition.hpp"
#include "Action.hpp"

constexpr int8_t SECTION_STATE_COMPLETED = -1;
constexpr int8_t SECTION_STATE_CONDITION = -2;
constexpr int8_t SECTION_STATE_REPEAT = -3;

class Section {
public:
    bool conditional = false;
    Condition condition;
    bool repeatEnabled = false;
    Condition repeatCondition;
    int numActions;
    Action actions[SR_MAX_SCRIPT_SECTION_ACTIONS];

    void setup(SetupContext* context);
    void execute(AutomationContext* context);
    bool loop(AutomationContext* context);
    void accessoryUpdated(unsigned int address, uint8_t accessoryState);
    void onRBusDataChanged(uint8_t groupId, uint8_t* data, uint16_t length);
    void onCanDetector(uint16_t nld, uint16_t addr, uint8_t port, uint8_t type, uint16_t value1, uint16_t value2);
    void onGPIOPinChange(uint8_t pin, bool active);
private:
    int state = -1;

    void executeNext(AutomationContext* context);  
};