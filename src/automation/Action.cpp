#include "Action.hpp"

void Action::setup(SetupContext* context) {
  if (type == ActionType::SetGPIO) {
    context->prepareGPIOPWrite(this->input1);
  } 
}

bool Action::loop(AutomationContext* context) {
  if (type == ActionType::Wait) {
    if (millis() - state > input1) {
      context->monitor->println("Executing wait completed");
      return true;
    }
    return false;
  }
  return true;
}

void Action::execute(AutomationContext* context) {
  switch(type) {
    case ActionType::Wait:
      context->monitor->print("Executing wait (");
      context->monitor->print(input1);
      context->monitor->println(") ..");
      state = millis();    
      break;
    case ActionType::SetAccessory:
      context->monitor->println("Execute set accessory ");
      context->sendZ21Packet(
        Z21.setAccessory(input1 - 1, input2 == 1 ? 0 : 1, true, true)
      );
      break;
    case ActionType::SetGPIO:
      context->monitor->println("Execute set gpio pin ");
      digitalWrite(input1, input2);
      break;
  }
}