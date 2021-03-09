#include <Arduino.h>
#include <Ethernet.h>
#include <ArduinoJson.h>

/* ArdunioJSON UDP Digistar Ping Pong ( TCP Version )
- Receive this TCPpacket:
{"count":1}
Then deserialize, parse, increment count and send JSON back to sender IP. 
Other host (Digistar) does the same until value = set cut off amount.
-------------------------------------------------------------------------------*/

// Buffer for receiving data, This value can't be set by a variable due to compiler allocating memory pool. 
// # must match below for json DOC size and buffer clear
char packetBuffer[14];
byte packetBufferSize = 14;
int packetPointer = 0;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Physcial Hardware Address
IPAddress ip(1, 1, 1, 40); // Local Area Network Address Assigned
byte gateway[] = { 1, 1, 1, 100 }; // internet access via router
byte subnet[] = { 255, 255, 255, 0 }; //subnet mask
EthernetServer server(80); //server port

void setup() {  //-------------------------------------------------------------------------------

  Serial.begin(9600);
  while (!Serial) continue;

  Ethernet.begin(mac, ip, gateway, subnet);   //start Ethernet
  server.begin();
}
//============================================================================================
void loop() {  //=============================================================================

  EthernetClient client = server.available();         // Check for server availability
  if(client) {
    //IPAddress remote = client.remoteIP();          // Record the sender's address
    //Serial.println(remote);

    for (packetPointer = 0; client.connected() && client && packetPointer <= packetBufferSize; ++packetPointer ){
      packetBuffer[packetPointer] = client.read();
    }
    
    //Serial.println(packetBuffer);
    //client.println(packetBuffer);

    // Deserialize the JSON document
    StaticJsonDocument<14> doc;
    DeserializationError error = deserializeJson(doc, packetBuffer);
    // Test if parsing succeeds.
    if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
    }
    else {
      // Fetch values from new json doc
      int count = doc["count"];  
      //You might be tempted to ++ object inside json doc itself but Arduino author stresses NOT to use json doc as a useable data structure to void memory leaks.
      doc["count"] = ++count;

      serializeJson(doc, Serial); // JSON doc as a single string for debugging.
      Serial.println();  // Start a new line
      
      serializeJson(doc, client);
    }
    // Clear all old data from packetBuffer
    for (unsigned int i = 0; i <= packetPointer; i++) { packetBuffer[i] = '\0'; }
    }
  }