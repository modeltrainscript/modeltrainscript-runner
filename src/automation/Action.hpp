#pragma once

#include "Context.hpp"

enum class ActionType: uint8_t { Wait = 1, SetAccessory = 2, SetGPIO = 3 };

class Action {
public:
    ActionType type;
    long input1;  // Wait: timeout, SetAccessory: address, SetGPIO: pin
    long input2; // SetAccessory: thrown = 1, SetGPIO active = 1

    void setup(SetupContext* context);
    bool loop(AutomationContext* context);
    void execute(AutomationContext* context);
private:
    long state;    
};