#pragma once

#include "Context.hpp"
#include "Condition.hpp"
#include "Section.hpp"

class Script {
public:        
    Condition event;
    int numSections;    
    Section sections[SR_MAX_SCRIPT_SECTIONS];

    bool running;

    void execute(Monitor* monitor);
    void loop(AutomationContext* context);   

    void feedbackChanged(Monitor* monitor, uint8_t address, uint8_t port, bool active);
private:
    int activeSection;

    void executeNext(Monitor* monitor);
};
