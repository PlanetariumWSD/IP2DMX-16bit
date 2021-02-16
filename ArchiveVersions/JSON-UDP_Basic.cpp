#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <ArduinoJson.h>

/* ArdunioJSON UDP Basic - Receive this udp packet:
{"sensor":"gps","time":1351824120,"data":[48.756080,2.302038]}
Then deserialize, parse and print values via serial.
-------------------------------------------------------------------------------*/

// Buffers for receiving and sending data
// This value can't be a variable due to compiler. 
// # must match below for json DOC size and buffer clear
char packetBuffer[200];
char ReplyBuffer[] = "OK";   // a string to send back

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Physcial Hardware Address
IPAddress ip(1, 1, 1, 41); // Local Area Network Address Assigned
unsigned int localPort = 8888; // local port to listen on
EthernetUDP Udp;    // An EthernetUDP instance to let us send and receive packets over UDP

void setup() {  //-------------------------------------------------------------------------------

  Serial.begin(9600);
  while (!Serial) continue;

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
    Serial.println("Packet Recieved:");
    Serial.println(packetBuffer);

    // Deserialize the JSON document
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, packetBuffer);
    // Test if parsing succeeds.
    if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
    }

    // Fetch values from new json doc
    const char* sensor = doc["sensor"];
    long time = doc["time"];
    double latitude = doc["data"][0];
    double longitude = doc["data"][1];

    // Print values manually (option).
    Serial.println(sensor);
    Serial.println(time);
    Serial.println(latitude, 6);
    Serial.println(longitude, 6);

    Serial.println("From JSON doc:");
    serializeJson(doc, Serial); // JSON doc as a single string  {"sensor":"gps","time":1351824120,"data":[48.756080,2.302038]}
    Serial.println();  // Start a new line
    serializeJsonPretty(doc, Serial);  // Generate the prettified JSON and send it to the Serial port.

    //-------------------------------------------------------------------------------
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());   // send a reply to the IP address and port that sent us the packet we received
    Udp.write(ReplyBuffer);
    Udp.endPacket();

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());   // send a reply to the IP address and port that sent us the packet we received
    // Udp.write(packetBuffer);
    serializeJson(doc, Udp);
    Udp.endPacket();

    // Clear all old data from packetBuffer
    for (unsigned int i = 0; i <= packetSize; i++) { packetBuffer[i] = '\0'; }
    // Important ArduinoJSON: No need to clear doc. It's automatic before a deserialization.
  }

  delay(100);
}