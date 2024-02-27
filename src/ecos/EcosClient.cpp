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

#include "EcosClient.hpp"
#include "EcosConst.hpp"
#include "EcosData.hpp"

void EcosClient::setup(Monitor* monitor) {
    this->monitor = monitor;
}

void EcosClient::loop() {
    if (socket.connected()) {
        readMessages();
    }
}

bool EcosClient::connected() {
    return socket.connected();
}

bool EcosClient::connect(IPAddress address, uint16_t port) {
    if (socket.connected()) {
        disconnect();
    }

    monitor->print("connecting to ECoS: " );
    monitor->print(address.toString());
    monitor->print(":" );
    monitor->println(port);

    if (!socket.connect(
        address, 
        port
        )
    ) {
        monitor->println("connection failure");    
        return false;    
    } else {
        monitor->println("connected to ECoS, registering views");

        if (!sendReply(translator.request(ECoS_Ids::ECoS, ECoS_Options::View))) {
            return false;
        }

        if (!sendReply(translator.get(ECoS_Ids::ECoS, ECoS_Options::Status))) {
            return false;
        }

        monitor->print("Status of command station: ");
        monitor->println(currentReply.data[0].value);

        monitor->println("Wait 5 sec for system initialisation");

        delay(5000);

        monitor->println("Resume initialisation");

        monitor->println("Query accessories");
        if (!sendReply(translator.queryObjects(ECoS_Ids::AccessoryManager, ECoS_Options::Address))) {
            return false;
        }        

        numberAccesories = 0;
        for(int i=0; i < currentReply.numData; ++i) {
            accessories[i].id = currentReply.data[i].id;
            accessories[i].address = String(currentReply.data[i].value).toInt();
            numberAccesories += 1;
        }

        monitor->print("Register view for accessories, total: ");
        monitor->println(numberAccesories);
        for(int i=0; i < numberAccesories; ++i) {        
            if(!sendReply(translator.request(accessories[i].id, ECoS_Options::View))) {
                return false;
            }
        }

        monitor->println("Query feedback modules");
        if(!sendReply(translator.queryObjects(ECoS_Ids::FeedbackManager, ECoS_Options::Ports))) {
            return false;
        }
        
        numberFeedbackModules = 0;
        for(int i=0; i < currentReply.numData; ++i) {        
            feedbackModules[i].id = currentReply.data[i].id;
            feedbackModules[i].ports = String(currentReply.data[i].value).toInt();
            numberFeedbackModules += 1;
        }

        monitor->print("Register view for feedback modules, total: ");
        monitor->println(numberFeedbackModules);
        for(int i=0; i < numberFeedbackModules; ++i) {      
            if(!sendReply(translator.request(feedbackModules[i].id, ECoS_Options::View))) {
                return false;
            }
        }

        monitor->println("ECoS connection ready");
        return true;
    }
}

void EcosClient::disconnect() {
     socket.stop();
}

bool EcosClient::requestAccesory(uint16_t address) {
    for(int i=0; i < numberAccesories; ++i) {     
        if (accessories[i].address == address) {
            sendReply(translator.get(accessories[i].id, ECoS_Options::State));
            return currentReply.data[0].value == ECoS_Values::On;
        }
    }
    return false;
}

void EcosClient::setAccessory(uint16_t address, bool on) {
    for(int i=0; i < numberAccesories; ++i) {     
        if (accessories[i].address == address) {
            sendReply(translator.set(accessories[i].id, ECoS_Options::State, on ? ECoS_Values::On : ECoS_Values::Off));
        }
    }    
}

bool EcosClient::requestFeedback(uint16_t address, uint8_t port) {
    for(int i=0; i < numberFeedbackModules; ++i) {     
        if (feedbackModules[i].id == address + 199) {
            sendReply(translator.get(feedbackModules[i].id, ECoS_Options::State));
            int value = (int) strtol( currentReply.data[0].value.c_str(), NULL, 16);

            int mask = 1 << (port - 1);
            bool active = (value & mask) == mask;
            return active;
        }
    }
    return false;
}

bool EcosClient::sendReply(EcosRequest request) {
    long sendTime = millis();

    socket.write(request.message().c_str());

    while(socket.connected()) {
        while(socket.connected() && !socket.available()) {
            delay(50);

            if((millis() - sendTime) > 2000) {
                monitor->println("timeout occured, disconnecting");

                disconnect();
                return false;
            }
        }

        bool foundReply = readMessages();
        if(foundReply && currentReply.request.equals(request.message())) {
            return true;
        }
            delay(50);
            delay(50);
        delay(50);
    }
    return true;
}

bool EcosClient::readMessages() {
    bool gotReply = false;

    while(this->socket.available()) {    
        char c = socket.read();

        if (c == '\n') {
            // Serial.println(currentLine);

            if (currentLine.startsWith("<REPLY ")) {
                currentReply.numData = 0;
                readingReply = true;

                String request = currentLine.substring(7, currentLine.length() - 2);
                currentReply.request = request;
            } else if (currentLine.startsWith("<EVENT ")) {
                currentEvent.numData = 0;
                readingReply = false;

                String idString = currentLine.substring(7, currentLine.length() - 2);
                currentEvent.id = idString.toInt();
            } else if (currentLine.startsWith("<END ")) {
                if (readingReply) {
                    gotReply = true;
                } else {
                    handleEvent();
                }
            } else if (readingReply) {
                readEcosData(&currentReply.data[currentReply.numData]);
                currentReply.numData += 1;
            } else if (!readingReply) {
                readEcosData(&currentEvent.data[currentEvent.numData]);
                currentEvent.numData += 1;
            }

            currentLine = "";
        } else {
            currentLine += c;
        }
    }

    return gotReply;
}

void EcosClient::readEcosData(EcosData* data) {
    int space = currentLine.indexOf(" ");
    if (space > -1) {
        data->id = currentLine.substring(0, space).toInt();

        String optionValue = currentLine.substring(space + 1, currentLine.length());
        int bracket = optionValue.indexOf("[");
        if (bracket > -1) {
            data->option = optionValue.substring(0, bracket);
            data->value = optionValue.substring(bracket + 1, optionValue.length() - 2);
        } else {
            data->option = optionValue;
            data->value = "";
        }
    } else {
        data->id = currentLine.toInt();
        data->option = "";
        data->value = "";
    }
}

void EcosClient::handleEvent() {
    for(int i=0; i < numberFeedbackModules; ++i) {
        FeedbackModule module = feedbackModules[i];

        if (module.id == currentEvent.id) {
            handleFeedbackEvent(&module);
        }
    }
}

void EcosClient::handleFeedbackEvent(FeedbackModule* module) {
    for(int i = 0; i < currentEvent.numData; ++i) {
        int value = (int) strtol( currentEvent.data[i].value.c_str(), NULL, 16);

        for(int port=0; port < module->ports; ++port) {
            int mask = 1 << port;
            bool active = (value & mask) == mask;
            feeedbackChanged(module->id - 199, port + 1, active);
        }
    }
}
