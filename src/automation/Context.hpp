#pragma once

#include <Arduino.h>

#include "monitor/Monitor.hpp"
#include "ecos/EcosClient.hpp"

constexpr u_int8_t SR_MAX_SCRIPTS = 5;
constexpr u_int8_t SR_MAX_SCRIPT_SECTIONS = 5;
constexpr u_int8_t SR_MAX_SCRIPT_SECTION_ACTIONS = 5;

class AutomationContext {
public:
    Monitor* monitor;
    EcosClient* client;

    AutomationContext(Monitor* monitor, EcosClient* client);
};