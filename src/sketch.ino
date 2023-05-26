#include <WiFi.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include <WifiServer.h>
#include <ESPmDNS.h>

#include "Config.h"
#include "z21/Z21.hpp"
#include "automation/AutomationData.hpp"
#include "io/Serializer.hpp"
#include "io/Repository.hpp"
#include "monitor/Monitor.hpp"

// UPD Setup Z21
const int          SERVER_PORT = 21105;

WiFiUDP            UDP;

uint8_t            RX_UDP_BUFFER[Z21Packet::MAX_PACKET_SIZE + 10]{};
int                gUDPLeftToRead = 0;
uint8_t*           gUDPReadTo = RX_UDP_BUFFER;

// Management API
constexpr size_t   MANAGEMENT_API_BUFFER_SIZE = 1024;
char               gApiBuffer[MANAGEMENT_API_BUFFER_SIZE]{};
WiFiServer         gAPIServer(80);

// Z21 
constexpr uint     Z21COMMAND_TIMEOUT = 55000;

unsigned long      gZ21LastCommandSendTime;

// GPIO
int                gNumMonitoredGPIOPins = 0;
uint8_t            gMonitorGPIOPins[10]{};
uint8_t            gMonitorGPIOPinStates[10]{};

// Data
Monitor            gMonitor;

Z21Config          gZ21Config;
AutomationData     gAutomationData;
AutomationContext  gAutomationContext(&gMonitor);
Serializer         gSerializer;

void setup()
{
    Serial.begin(115200);

    delay(100);

    // Connect to WiFi
    WiFi.disconnect(true);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname(WIFI_HOSTNAME);
    WiFi.begin(WIFI_SSID, WIFI_PASSWD);
    
    while ( WiFi.status() != WL_CONNECTED ) {
        delay(500);
        Serial.print(".");
    }    

    Serial.print("\nConnected to WiFi, ip: ");    
    Serial.println(WiFi.localIP());

    // Setup service advertisement
    if (!MDNS.begin(WIFI_HOSTNAME)) {
        Serial.println("Error starting mDNS");
        return;
    }
 
    MDNS.addService("mts", "tcp", 80);

    // Prepare stored data
    readData();
    setupData();

    // Setup Z21 connection
    setupZ21();
    gAutomationContext.sendZ21Packet = sendPacket;

    // Setup GPIO
    gAutomationContext.readGPIOPin = readGPIOPin;

    // Setup management api
    setupApiServer();
}

void loop()
{
    // Update pins
    loopGPIO();

    // Update Automation
    gAutomationData.loop(&gAutomationContext);

    // Update management api
    loopApiServer();

    // Update Z21 Connection    
    readZ21UDP();

    if (millis() - gZ21LastCommandSendTime > Z21COMMAND_TIMEOUT) {
        sendPacket(Z21.getSerialNumber());       
    }
    
    // Pause loop
    delay(50);
}

/////////////////////////////////////////////////////////////////////
// Management API
/////////////////////////////////////////////////////////////////////
void setupApiServer() {
    gAPIServer.begin();  
}

void loopApiServer() {
      WiFiClient client = gAPIServer.available();

 	if (client) {        
         while (client.connected()) {
             if (client.available()) {  
                 Serial.println("client connected");

                 char command = client.read();
                 if (command == 'i') {
                    Serial.println("Api: Info");
                    client.write(DATA_VERSION);
                    client.stop();
                 } else if (command == 'r') {
                    Serial.println("Api: Read");

                    StreamWriter writer(&client);
                    gSerializer.write(&gZ21Config, &gAutomationData, &writer);
                    client.stop();
                 } else if (command == 'w') {
                    Serial.println("Api: Write");

                    StreamReader reader(&client);
                    gSerializer.read(&gZ21Config, &gAutomationData, &reader);

                    Serial.print("Data received: ");
                    Serial.println(reader.bytesRead);

                    Serial.println("Initializing data");
                    setupData();

                    Serial.println("client data read, writing");
                    writeData();

                    Serial.println("Configuration written, reconnect z21");
                    connectZ21();

                    client.write(reader.bytesRead);
                    client.write((reader.bytesRead) >> 8);
                    client.stop();
                 } else if (command == 'm') {
                    Serial.println("Api: Monitor");
                    gMonitor.write(&client);
                    client.stop();
                    Serial.println("Api: End Monitor");
                 } else { 
                    Serial.print("Error: Unrecognized command ");
                    Serial.println(command);
                    client.stop();
                 }
             }
         }         
     }
}

/////////////////////////////////////////////////////////////////////
// Z21
/////////////////////////////////////////////////////////////////////
void setupZ21() {
    gMonitor.println("Connecting to Z21");
    Z21.onAccessoryInfo = onAccessoryInfo;
    Z21.onRBusDataChanged = onRBusDataChanged;
    Z21.onCanDetector = onCanDetector;

    connectZ21();
    gMonitor.println("Connected to z21");
}

void connectZ21() {
    gMonitor.print("Z21 Configuration: ");
    gMonitor.print(gZ21Config.ipAddress[0]);
    gMonitor.print(".");
    gMonitor.print(gZ21Config.ipAddress[1]);
    gMonitor.print(".");
    gMonitor.print(gZ21Config.ipAddress[2]);
    gMonitor.print(".");
    gMonitor.print(gZ21Config.ipAddress[3]);
    gMonitor.print(":");
    gMonitor.println(gZ21Config.port);

    UDP.begin(SERVER_PORT);        
    gMonitor.println("UDP Connected.");   

    sendPacket(Z21.logoff());
    delay(100);
    sendPacket(Z21.getSerialNumber());
    sendPacket(Z21.setBroadcastFlags(
        Z21_Broadcast::STATUS_LOCO_TURNOUT + Z21_Broadcast::RMBUS_DATACHANGED + Z21_Broadcast::CAN_DETECTOR )
    );    
     delay(100);
}

void sendPacket(Z21Packet& packet) {
    UDP.beginPacket(
        IPAddress(192, 168, 2, 201), 21105
        );
    UDP.write(packet.data, packet.length);
    UDP.endPacket();
    
    gZ21LastCommandSendTime = millis();
}

void readZ21UDP() {
    if (gUDPLeftToRead <= 0) {
        gUDPLeftToRead = UDP.parsePacket();
        
        if (gUDPLeftToRead <= 0) 
            return;

        if (gUDPLeftToRead < Z21Packet::MIN_PACKET_SIZE || gUDPLeftToRead > Z21Packet::MAX_PACKET_SIZE) {
            gUDPLeftToRead = 0; // skip this packet
        }

        if (gUDPLeftToRead <= 0) 
            return;
            
        gUDPReadTo = RX_UDP_BUFFER;
    }
    
    int read = UDP.read(gUDPReadTo, gUDPLeftToRead);
    gUDPLeftToRead -= read;
    gUDPReadTo += read;

    if (gUDPLeftToRead <= 0) {
        Z21.processPacket(RX_UDP_BUFFER);
    }
}

/////////////////////////////////////////////////////////////////////
// Z21 callbacks
/////////////////////////////////////////////////////////////////////
void onAccessoryInfo(uint16_t address, uint8_t accessoryState) {
    gMonitor.println("onAccessoryInfo");
    gAutomationData.accessoryUpdated(address, accessoryState);
}

void onRBusDataChanged(uint8_t groupId, uint8_t* data, uint16_t length) {
    gMonitor.println("onRBusDataChanged");
    gAutomationData.onRBusDataChanged(groupId, data, length);
}

void onCanDetector(uint16_t nld, uint16_t addr, uint8_t port, uint8_t type, uint16_t value1, uint16_t value2) {
    gMonitor.println("onCanDetector");
    gAutomationData.onCanDetector(nld, addr, port, type, value1, value2);
}

/////////////////////////////////////////////////////////////////////
// GPIO
/////////////////////////////////////////////////////////////////////
void readGPIOPin(u_int8_t pin) {
    for (int i = 0; i < gNumMonitoredGPIOPins; ++i) {
        if (gMonitorGPIOPins[i] == pin) {
            gMonitorGPIOPinStates[i] = -1; 
        }
    }  
}

void loopGPIO() {
    for (int i = 0; i < gNumMonitoredGPIOPins; ++i) {
        int active = digitalRead(gMonitorGPIOPins[i]);

        if (active != gMonitorGPIOPinStates[i]) {
            gMonitor.print("onGPIOPinChange ");
            gMonitor.print(gMonitorGPIOPins[i]);
            gMonitor.print(" to ");
            gMonitor.println(active);
            gAutomationData.onGPIOPinChange(gMonitorGPIOPins[i], active);
            gMonitorGPIOPinStates[i] = active;
        }
    }
}

void prepareGPIOPRead(uint8_t pin) {
    gMonitor.print("Prepare gpio pin for read ");
    gMonitor.println(pin);

    gMonitorGPIOPins[gNumMonitoredGPIOPins] = pin;
    gMonitorGPIOPinStates[gNumMonitoredGPIOPins++] = -1;
    pinMode(pin, INPUT_PULLUP);
}

void prepareGPIOPWrite(uint8_t pin) {
    gMonitor.print("Prepare gpio pin for write ");
    gMonitor.println(pin);

    pinMode(pin, OUTPUT);
}

/////////////////////////////////////////////////////////////////////
// Data
/////////////////////////////////////////////////////////////////////
void setupData() {
    gMonitor.print("Number of scripts: ");
    gMonitor.println(gAutomationData.numScripts);

    gNumMonitoredGPIOPins = 0;

    SetupContext context;
    context.prepareGPIOPRead = prepareGPIOPRead;
    context.prepareGPIOPWrite = prepareGPIOPWrite;
    gAutomationData.setup(&context);
}

void readData() {
    RepositoryReader reader;
    reader.begin();
    gSerializer.read(&gZ21Config, &gAutomationData, &reader);    
    reader.end();
}

void writeData() {
    RepositoryWriter writer;
    writer.begin();
    gSerializer.write(&gZ21Config, &gAutomationData, &writer);
    writer.commit();
    writer.end();
}
