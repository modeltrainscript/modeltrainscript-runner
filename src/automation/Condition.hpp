#pragma once

#include "Context.hpp"

constexpr uint8_t RBUS_GROUP_SIZE = 10;

enum class ConditionType: uint8_t { Accessory = 1, Feedback = 2 };

constexpr int8_t CONDITION_STATE_PENDING = -1;

class Condition {
public:
    ConditionType type;

    /*
    accessory: decoder address
    rbus: module index
    can: module address
    gpio: pin
    */
    uint16_t address;
    /*
    rbus: port index
    can: port index
    */
    uint8_t input;
    bool active;

    bool evaluate(AutomationContext* context);

    bool feedbackChanged(uint8_t address, uint8_t port, bool active);
private:
};