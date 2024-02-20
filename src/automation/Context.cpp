#include "Context.hpp"

AutomationContext::AutomationContext(Monitor* monitor, EcosClient* client) {
    this->monitor = monitor;
    this->client = client;
}