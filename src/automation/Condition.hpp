#pragma once

#include "Context.hpp"

constexpr uint8_t RBUS_GROUP_SIZE = 10;

enum class ConditionType: uint8_t { Accessory = 1, FeedbackRBus = 2, FeedbackCan = 3, GPIO = 4 };

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

    void setup(SetupContext* context);
    void requestCurrentState(AutomationContext* context);
    void startEvaluation();
    bool loop(bool* result);
    void accessoryUpdated(unsigned int address, uint8_t accessoryState);
    void onRBusDataChanged(uint8_t groupId, uint8_t* data, uint16_t length);
    void onCanDetector(uint16_t nld, uint16_t addr, uint8_t port, uint8_t type, uint16_t value1, uint16_t value2);
    void onGPIOPinChange(uint8_t pin, bool active);
private:
    int state = CONDITION_STATE_PENDING;
};