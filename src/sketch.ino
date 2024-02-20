#include <WiFi.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include <WifiServer.h>
#include <ESPmDNS.h>

#include "Config.h"
#include "ecos/EcosClient.hpp"

#include "ecos/Ecos.hpp"
#include "ecos/EcosConst.hpp"

#include "automation/AutomationData.hpp"
#include "io/Serializer.hpp"
#include "io/Repository.hpp"
#include "monitor/Monitor.hpp"

// ECOS
EcosClient         gEcosClient;

// Management API
constexpr size_t   MANAGEMENT_API_BUFFER_SIZE = 1024;
char               gApiBuffer[MANAGEMENT_API_BUFFER_SIZE]{};
WiFiServer         gAPIServer(80);

// Data
Monitor            gMonitor;

Config             gConfig;
AutomationData     gAutomationData;
AutomationContext  gAutomationContext(&gMonitor, &gEcosClient);
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

    // Setup ECOS
    gEcosClient.feeedbackChanged = &onFeedbackChanged;
    gEcosClient.setup(&gMonitor);

    // Setup management api
    setupApiServer();
}

void loop()
{
    // Update Automation
    gAutomationData.loop(&gAutomationContext);

    // Update management api
    loopApiServer();

    // ECOS
    if (!gEcosClient.connected()) {
        gEcosClient.connect(gConfig.ipAddress, gConfig.port);
        delay(1000);
    } else {
        gEcosClient.loop();
    }    

    delay(100);
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
                    gSerializer.write(&gConfig, &gAutomationData, &writer);
                    client.stop();
                 } else if (command == 'w') {
                    Serial.println("Api: Write");

                    StreamReader reader(&client);
                    gSerializer.read(&gConfig, &gAutomationData, &reader);

                    Serial.print("Data received: ");
                    Serial.println(reader.bytesRead);

                    Serial.println("client data read, writing");
                    writeData();

                    Serial.println("Configuration written, reconnect command station");
                    gEcosClient.connect(gConfig.ipAddress, gConfig.port);

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

void onFeedbackChanged(uint8_t address, uint8_t port, bool active) {
    gAutomationData.feedbackChanged(&gMonitor, address, port, active);
}

/////////////////////////////////////////////////////////////////////
// Data
/////////////////////////////////////////////////////////////////////
void readData() {
    RepositoryReader reader;
    reader.begin();
    gSerializer.read(&gConfig, &gAutomationData, &reader);    
    reader.end();
}

void writeData() {
    RepositoryWriter writer;
    writer.begin();
    gSerializer.write(&gConfig, &gAutomationData, &writer);
    writer.commit();
    writer.end();
}
