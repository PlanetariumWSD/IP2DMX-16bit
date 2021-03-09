#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <ArduinoJson.h>

/* ArdunioJSON UDP Digistar Ping Pong
- Receive this udp packet:
{"count":1}
Then deserialize, parse, increment count and send JSON back to sender IP. 
Other host (Digistar) does the same until value = set cut off amount.
-------------------------------------------------------------------------------*/

// Buffer for receiving data, This value can't be set by a variable due to compiler allocating memory pool. 
// # must match below for json DOC size and buffer clear
char packetBuffer[14];

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Physcial Hardware Address
IPAddress ip(1, 1, 1, 40); // Local Area Network Address Assigned
unsigned int localPort = 8888; // local port to listen on
EthernetUDP Udp;    // An EthernetUDP instance to let us send and receive packets over UDP

void setup() {  //-------------------------------------------------------------------------------

  //Serial.begin(9600);
  //while (!Serial) continue;

  Ethernet.begin(mac, ip); //Start Ethernet
  Udp.begin(localPort);  // UDP Start
}
//============================================================================================
void loop() {  //=============================================================================

// CHECK FOR UDP PACKET ----------------
  unsigned int packetSize = Udp.parsePacket();  // if there's UDP data available, read a packet
  if (packetSize) {
    IPAddress remote = Udp.remoteIP();          // Record the sender's address
    Udp.read(packetBuffer, packetSize);         // read the packet into packetBuffer

    // Deserialize the JSON document
    StaticJsonDocument<14> doc;
    DeserializationError error = deserializeJson(doc, packetBuffer);
    // Test if parsing succeeds.
    if (error) {
    //Serial.print(F("deserializeJson() failed: "));
    //Serial.println(error.f_str());
    return;
    }
    else {
      // Fetch values from new json doc
      int count = doc["count"];  
      //You might be tempted to ++ object inside json doc itself but Arduino author stresses NOT to use json doc as a useable data structure to void memory leaks.
      doc["count"] = ++count;

      //serializeJson(doc, Serial); // JSON doc as a single string for debugging.
      //Serial.println();  // Start a new line
      
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());   // send a reply to the IP address and port that sent us the packet we received
      serializeJson(doc, Udp);
      Udp.endPacket();
    }

    // Clear all old data from packetBuffer
    for (unsigned int i = 0; i <= packetSize; i++) { packetBuffer[i] = '\0'; }
  }

  //delay(100);
}