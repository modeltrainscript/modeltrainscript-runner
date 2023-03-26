#pragma once

#include <Arduino.h>

#include "monitor/Monitor.hpp"
#include "Z21/Z21.hpp"

constexpr u_int8_t SR_MAX_SCRIPTS = 5;
constexpr u_int8_t SR_MAX_SCRIPT_SECTIONS = 5;
constexpr u_int8_t SR_MAX_SCRIPT_SECTION_ACTIONS = 5;

class SetupContext {
public:
    void (*prepareGPIOPRead)(u_int8_t pin);
    void (*prepareGPIOPWrite)(u_int8_t pin);
};

class AutomationContext {
public:
    Monitor* monitor;

    AutomationContext(Monitor* stream);
    void (*sendZ21Packet)(Z21Packet& packet);
    void (*readGPIOPin)(u_int8_t pin);
};