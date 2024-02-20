/**
 ** This is Public Domain Software.
 **
 ** The author disclaims copyright to this source code.
 ** In place of a legal notice, here is a blessing:
 **
 **    May you do good and not evil.
 **    May you find forgiveness for yourself and forgive others.
 **    May you share freely, never taking more than you give.
 **/

#ifndef __ECOS_CLIENT_H__
#define __ECOS_CLIENT_H__

#include <WiFi.h>

#include "monitor/Monitor.hpp"

#include "Ecos.hpp"

class FeedbackModule {
public:    
    uint16_t id;
    uint8_t ports;
};

class Accessory {
public:
    uint16_t id;
    uint16_t address;
};

class EcosClient {
public:
    void setup(Monitor* monitor);

    void loop();

    bool connected();

    void connect(IPAddress address, uint16_t port);

    bool requestAccesory(uint16_t address);
    void setAccessory(uint16_t address, bool on);

    bool requestFeedback(uint16_t address, uint8_t port);

    void (*feeedbackChanged)(uint8_t address, uint8_t port, bool active);
private:
    Monitor* monitor;
    WiFiClient socket;
    EcosTranslator translator;

    uint8_t numberFeedbackModules;
    FeedbackModule feedbackModules[20];
    
    uint8_t numberAccesories;
    Accessory accessories[100];

    void sendReply(EcosRequest request);

    String currentLine;
    EcosReply currentReply;
    EcosEvent currentEvent;
    bool readingReply;
    void readEcosData(EcosData* data);

    bool readMessages();
    void handleEvent();   
    void handleFeedbackEvent(FeedbackModule* module);
};

#endif