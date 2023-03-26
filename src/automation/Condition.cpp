#include "Condition.hpp"

void Condition::setup(SetupContext* context) {
  if (type == ConditionType::GPIO) {
    context->prepareGPIOPRead(address);
  }
}

void Condition::requestCurrentState(AutomationContext* context) {
  switch(type) {
    case ConditionType::Accessory:
      context->sendZ21Packet(Z21.getAccessoryInfo(address - 1));
      break;
    case ConditionType::FeedbackRBus:
      context->sendZ21Packet(Z21.rbusGetData(address > RBUS_GROUP_SIZE ? 1 : 0));
      break;
    case ConditionType::FeedbackCan:
      context->sendZ21Packet(Z21.canDetector());
      break;
    case ConditionType::GPIO:
      context->readGPIOPin(address);
      break;
    }
}

void Condition::startEvaluation() {
  state = CONDITION_STATE_PENDING;
}

bool Condition::loop(bool* result) {
  *result = state;
  return state != CONDITION_STATE_PENDING;
}

void Condition::accessoryUpdated(unsigned int address, uint8_t accessoryState) {
  if (type == ConditionType::Accessory && this->address - 1 == address) {
    state = active ? accessoryState == 1 : accessoryState == 2;
  }
}

void Condition::onRBusDataChanged(uint8_t groupId, uint8_t* data, uint16_t length) {
  uint8_t expectedGroupId = address < RBUS_GROUP_SIZE ? 0 : 1;

  if (type == ConditionType::FeedbackRBus && expectedGroupId == groupId) {
    int index = address > RBUS_GROUP_SIZE ? address - 11 : address - 1;
    uint8_t moduleState = data[index];
    int mask = 1 << (input - 1);
    state = (moduleState & mask) == mask;
  }
}

void Condition::onCanDetector(uint16_t nld, uint16_t addr, uint8_t port, uint8_t type, uint16_t value1, uint16_t value2) {
  if (this->type == ConditionType::FeedbackCan &&
    type == 1 && this->address - 1 == addr && input - 1 == port) {
    bool free = value1 == 0x0000 || value1 == 0x0100;
    this->state = active != free;
  }
}

void Condition::onGPIOPinChange(uint8_t pin, bool active) {
  if (type == ConditionType::GPIO && address == pin) {
    state = this->active == active;
  }
}