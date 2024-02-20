#ifndef __ECOSDATA_H__
#define __ECOSDATA_H__

#include <Arduino.h>

class EcosRequest {
public:
    String action;
    u16_t id;
    String option;
    String value;

    String message();
};

class EcosData {
public:
    uint16_t id;
    String option;
    String value;
};

class EcosReply {
public:
    String request;
    int numData;
    EcosData data[200];
};

class EcosEvent {
public: 
    uint16_t id;
    int numData;
    EcosData data[200];
};

#endif