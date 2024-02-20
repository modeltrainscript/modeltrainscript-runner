#include "Action.hpp"

bool Action::loop(AutomationContext* context) {
    if (type == ActionType::Wait) {
        if (state == -1) {
            context->monitor->println("Execute wait");
            this->state = millis();  
        } else if (millis() - state > input1) {
            context->monitor->println("Executing wait completed");
            return true;
        }       
    } else if (type == ActionType::SetAccessory) {
        context->monitor->println("Execute set accessory");
        context->client->setAccessory(input1, input2 == 1);
        context->monitor->println("Accessory set");
        return true;
    }
    return false;    
}

void Action::execute() {
    if (type == ActionType::Wait) {
        state = -1;
    }
}

