#include "Condition.hpp"

bool Condition::evaluate(AutomationContext* context) {
    if  (type == ConditionType::Accessory) {
        bool currentState = context->client->requestAccesory(this->address);
        return currentState == active;
    } else if (type == ConditionType::Feedback) {
        bool currentState = context->client->requestFeedback(this->address, this->input);
        return currentState == active;
    }
    return false;
}

bool Condition::feedbackChanged(uint8_t address, uint8_t port, bool active) {
    if (type == ConditionType::Feedback) {
        return this->address == address && this->input == port && this->active == active;
    }
    return false;
}

