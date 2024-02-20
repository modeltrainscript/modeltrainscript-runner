#pragma once

#include "Context.hpp"
#include "Script.hpp"

class AutomationData {
public:
    int numScripts;
    Script scripts[SR_MAX_SCRIPTS];

    void loop(AutomationContext* context);

    void feedbackChanged(Monitor* monitor, uint8_t address, uint8_t port, bool active);
};