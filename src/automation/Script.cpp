#include "Script.hpp"

void Script::setup(SetupContext* context) {
  event.setup(context);

  for (int i=0; i < numSections; ++i) {      
      sections[i].setup(context);      
  }   

  event.startEvaluation();
}

void Script::loop(AutomationContext* context) {
  if (state == SCRIPT_STATE_COMPLETED) {
    bool evaluationResult;
    if (event.loop(&evaluationResult)) {
      if (evaluationResult) {
        context->monitor->println("Script Triggered");
        executeNext(context);
      } else {
        event.startEvaluation();
      }
    }
  } else if (this->state > SCRIPT_STATE_COMPLETED) {
      if(this->sections[this->state].loop(context)) {
          executeNext(context);
      }
  }
}

void Script::accessoryUpdated(unsigned int address, uint8_t accessoryState) {
  if (state == SCRIPT_STATE_COMPLETED) {
    event.accessoryUpdated(address, accessoryState);
  } else {
    sections[state].accessoryUpdated(address, accessoryState);
  }
}

void Script::onRBusDataChanged(uint8_t groupId, uint8_t * data, uint16_t length) {
  if (state == SCRIPT_STATE_COMPLETED) {
    event.onRBusDataChanged(groupId, data, length);
  } else {
    sections[state].onRBusDataChanged(groupId, data, length);
  }
}

void Script::onCanDetector(uint16_t nld, uint16_t addr, uint8_t port, uint8_t type, uint16_t value1, uint16_t value2) {
  if (state == SCRIPT_STATE_COMPLETED) {
    event.onCanDetector(nld, addr, port, type, value1, value2);
  } else {
    sections[state].onCanDetector(nld, addr, port, type, value1, value2);
  }
}

void Script::onGPIOPinChange(uint8_t pin, bool active) {
  if (state == SCRIPT_STATE_COMPLETED) {
    event.onGPIOPinChange(pin, active);
  } else {
    sections[state].onGPIOPinChange(pin, active);
  }
}

void Script::executeNext(AutomationContext* context) {
    if (state < this->numSections - 1) {
        state += 1;
        sections[this->state].execute(context);
    } else {
        state = SCRIPT_STATE_COMPLETED;
        event.startEvaluation();
        context->monitor->println("Script completed");
    }
}