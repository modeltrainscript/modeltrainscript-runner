#include "AutomationData.hpp"

void AutomationData::setup(SetupContext* context) {
  for (int i=0; i < numScripts; ++i) {
    scripts[i].setup(context);        
  }   
}

void AutomationData::loop(AutomationContext* context) {
  for (int i=0; i < numScripts; ++i) {
    scripts[i].loop(context);        
  }    
}

void AutomationData::accessoryUpdated(unsigned int address, uint8_t accessoryState) {
    for (int i=0; i < numScripts; ++i) {
        scripts[i].accessoryUpdated(address, accessoryState);
    }
}

void AutomationData::onRBusDataChanged(uint8_t groupId, uint8_t* data, uint16_t length) {
    for (int i=0; i < numScripts; ++i) {
        scripts[i].onRBusDataChanged(groupId, data, length);
    }
}

void AutomationData::onCanDetector(uint16_t nld, uint16_t addr, uint8_t port, uint8_t type, uint16_t value1, uint16_t value2) {
    for (int i=0; i < numScripts; ++i) {
        scripts[i].onCanDetector(nld, addr, port, type, value1, value2);
    }    
}

void AutomationData::onGPIOPinChange(uint8_t pin, bool active) {
    for (int i=0; i < numScripts; ++i) {
        scripts[i].onGPIOPinChange(pin, active);
    }      
}