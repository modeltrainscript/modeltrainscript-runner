#include "AutomationData.hpp"

void AutomationData::loop(AutomationContext* context) {
  for (int i=0; i < numScripts; ++i) {
    scripts[i].loop(context);        
  }    
}

void AutomationData::feedbackChanged(Monitor* monitor, uint8_t address, uint8_t port, bool active) {
  for (int i=0; i < numScripts; ++i) {
    scripts[i].feedbackChanged(monitor, address, port, active);        
  }      
}
