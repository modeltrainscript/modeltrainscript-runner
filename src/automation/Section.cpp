#include "Section.hpp"

bool Section::loop(AutomationContext* context) {
    Action* action = &actions[state];

    if (state == SECTION_STATE_CONDITION) {
      if (!condition.evaluate(context)) {
        context->monitor->println("Section condition not met");
        return true;
      } else {
        state = SECTION_STATE_COMPLETED;  
        executeNext(context->monitor);
      } 
    } else if (state == SECTION_STATE_REPEAT) {
      if (!repeatCondition.evaluate(context)) {
        context->monitor->println("Section repeat condition not met");
        return true;
      } else {
        state = SECTION_STATE_COMPLETED;  
        executeNext(context->monitor);
      }     
    } else {      
      if (action->loop(context)) {
          executeNext(context->monitor);  
      }
    }

    return state >= numActions;
}

void Section::execute(Monitor* monitor) {
  monitor->println("Execute section");

  if (conditional) {    
    state = SECTION_STATE_CONDITION;
  } else {
    state = SECTION_STATE_COMPLETED;  
    executeNext(monitor);
  }
}

void Section::executeNext(Monitor* monitor) {
  state += 1;
  if (state < numActions) {    
    actions[state].execute();
  } else if (repeatEnabled) {
    state = SECTION_STATE_REPEAT;
  } else {
    monitor->println("Section completed");
  }
}

