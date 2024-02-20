#pragma once

#include "Context.hpp"

enum class ActionType: uint8_t { Wait = 1, SetAccessory = 2 };

class Action {
public:
    ActionType type;
    long input1;  // Wait: timeout, SetAccessory: address
    long input2; // SetAccessory: thrown = 1

    bool loop(AutomationContext* context);
    void execute();
private:
    long state;
};