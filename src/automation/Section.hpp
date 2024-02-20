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

    void execute(Monitor* monitor);
    bool loop(AutomationContext* context);

private:
    int state = -1;

    void executeNext(Monitor* monitor);
};