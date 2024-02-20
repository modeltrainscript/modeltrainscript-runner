#include "Script.hpp"

void Script::loop(AutomationContext* context) {
  if (running) {
    Section* section = &sections[activeSection];

    if (section->loop(context)) {
      executeNext(context->monitor);  
    }
  }
}

void Script::execute(Monitor* monitor) {
  if (!running) {
    monitor->print("Executing script ");
    monitor->println(numSections);

    running = true;
    activeSection = -1;
    executeNext(monitor);
  }
}

void Script::executeNext(Monitor* monitor) {
  activeSection += 1;

  if (activeSection < numSections) {    
    sections[activeSection].execute(monitor);
  } else {
    running = false;
    monitor->println("Script completed");
  }
}

void Script::feedbackChanged(Monitor* monitor, uint8_t address, uint8_t port, bool active) {
  if (!running) {
    if (event.feedbackChanged(address, port, active)) {
      execute(monitor);
    }
  }
}
