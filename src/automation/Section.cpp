#include "Section.hpp"

void Section::setup(SetupContext* context) {
    if (repeatEnabled) {
      repeatCondition.setup(context);
    }

    for (int i=0; i < numActions; ++i) {
        actions[i].setup(context);        
    }   
}

void Section::execute(AutomationContext* context) {
  if (conditional) {
    state = SECTION_STATE_CONDITION;
    condition.startEvaluation();
    condition.requestCurrentState(context);
  } else {
    state = SECTION_STATE_COMPLETED;
    executeNext(context);
  }
}

bool Section::loop(AutomationContext* context) {
  if (state == SECTION_STATE_CONDITION) {
    bool evaluationResult;
    if (condition.loop(&evaluationResult)) {
      context->monitor->print("Condition: ");
      context->monitor->println(evaluationResult);

      state = SECTION_STATE_COMPLETED;
      if (evaluationResult) {
        executeNext(context);
      }
    }    
  } else if (state == SECTION_STATE_REPEAT) {
    bool evaluationResult;
    if (repeatCondition.loop(&evaluationResult)) {
      context->monitor->print("Repeat: ");
      context->monitor->println(evaluationResult);

      state = SECTION_STATE_COMPLETED;
      if (evaluationResult) {
        executeNext(context);
      }
    }
  } else if (state > -1) {
    if (actions[state].loop(context)) {
      executeNext(context);
    }
  }
  return state == SECTION_STATE_COMPLETED;
}

void Section::accessoryUpdated(unsigned int address, uint8_t accessoryState) {
  if (this->state == SECTION_STATE_CONDITION) {
    this->condition.accessoryUpdated(address, accessoryState);
  } else if (this->state == SECTION_STATE_REPEAT) {
    this->repeatCondition.accessoryUpdated(address, accessoryState);
  }
}

void Section::onRBusDataChanged(uint8_t groupId, uint8_t * data, uint16_t length) {
  if (this->state == SECTION_STATE_CONDITION) {
    this->condition.onRBusDataChanged(groupId, data, length);
  } else if (this->state == SECTION_STATE_REPEAT) {
    this->repeatCondition.onRBusDataChanged(groupId, data, length);
  }
}

void Section::onCanDetector(uint16_t nld, uint16_t addr, uint8_t port, uint8_t type, uint16_t value1, uint16_t value2) {
  if (this->state == SECTION_STATE_CONDITION) {
    this->condition.onCanDetector(nld, addr, port, type, value1, value2);
  } else if (this->state == SECTION_STATE_REPEAT) {
    this->repeatCondition.onCanDetector(nld, addr, port, type, value1, value2);
  }
}

void Section::onGPIOPinChange(uint8_t pin, bool active) {
  if (this->state == SECTION_STATE_CONDITION) {
    this->condition.onGPIOPinChange(pin, active);
  } else if (this->state == SECTION_STATE_REPEAT) {
    this->repeatCondition.onGPIOPinChange(pin, active);
  }
}

void Section::executeNext(AutomationContext* context) {
  if (this -> state < this -> numActions - 1) {
    this -> state += 1;
    this -> actions[this -> state].execute(context);
  } else if (this -> numActions > 0 && this -> repeatEnabled) {
    this -> repeatCondition.startEvaluation();
    this->repeatCondition.requestCurrentState(context);
    this -> state = SECTION_STATE_REPEAT;
  } else {
    this -> state = SECTION_STATE_COMPLETED;
  }
}