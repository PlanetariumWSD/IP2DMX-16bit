#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <DmxSimple.h>
#include <ArduinoJson.h>
/*=============================================================
    BASIC TESTING OF HARDWARE/LIBRARY INTERFACE
   (20) 16 bit nodes (or light fixtures) addressed as (40) 8-bit DMX channels [chn1 course, chn1 fine]
   Example: Node 1 is controlled by (2) DMX channels -> DMX chn1 (node 1 course) & DMX chn2 (node 1 fine)
   Communication JSON Object Fields: 
   - Input lenght is 82 per node, 40 bytes minimum datastrucuture, 48 bytes recommended.
   - 40 x 20 nodes 800 bytes + ~10% = 864
    {
       node: (uint8_t), 255
       val: (uint16_t), 65536
       dur: (uint24_t), 16777215
       ramp: (uint8_t), 255
       loop: (uint8_t), 255
      }

{
  "1": {
    "val": 65536,
    "dur": 16777215,
    "ramp": 255,
    "loop": 255
  }
    "2": {
    "val": 65535,
    "dur": 16777214,
    "ramp": 254,
    "loop": 254
  }
}

{"1": {"val":65536,"dur":16777215,"ramp":255,"loop": 255}}

===========================================================*/
uint8_t node = 0;
uint16_t val[20];
uint24_t dur[20];
uint8_t ramp[20];
uint8_t loop[20];

// Buffer to hold incoming packet
// This value can't be a variable due to compiler. Hard # must match below for json DOC size and buffer clear
char packetBuffer[864];
char ReplyBuffer[] = "OK";                             // a string to send back

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xFD };   // Physcial Hardware Address
IPAddress ip(1, 1, 1, 40);                             // Local Area Network Address Assigned
unsigned int localPort = 8888;                         // local port to listen on
EthernetUDP Udp;                                       // An EthernetUDP instance to let us send and receive packets over UDP

//============================================================================================
void setup() {  //============================================================================
Serial.begin(9600);                                    //Debugging purposes
while (!Serial) continue;
Ethernet.begin(mac, ip);                               //Start Ethernet
Udp.begin(localPort);                                  // UDP Start

}
//============================================================================================
void loop() {  //=============================================================================
// CHECK FOR UDP PACKET ----------------
  unsigned int packetSize = Udp.parsePacket();         // if there's UDP data available, read a packet
  if (packetSize) {
    IPAddress remote = Udp.remoteIP();                 // Record the sender's address
    Udp.read(packetBuffer, packetSize);                // read the packet into packetBuffer
    Serial.println("Packet Recieved:");
    Serial.println(packetBuffer);

    StaticJsonDocument<864> doc;                        // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, packetBuffer);
    if (error) {                                       // Test if parsing succeeds.
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    else {
      // Fetch values from new json doc
      // Most of the time, you can rely on the implicit casts.
      // In other case, you can do doc["time"].as<long>();
      //const char* sensor = doc["sensor"];
      //long time = doc["time"];
      //double latitude = doc["data"][0];
      //double longitude = doc["data"][1];


      double longitude = doc[1][1];

      int count = doc["count"];
      doc["count"] = ++count;
      

      // Print values.
      //Serial.println(sensor);
      //Serial.println(time);
      //Serial.println(latitude, 6);
      //Serial.println(longitude, 6);
      //-------------------------------------------------------------------------------

      //Serial.println("From JSON doc:");
      serializeJson(doc, Serial); // JSON doc as a single string  {"sensor":"gps","time":1351824120,"data":[48.756080,2.302038]}
      Serial.println();  // Start a new line
      //serializeJsonPretty(doc, Serial);  // Generate the prettified JSON and send it to the Serial port.

      //-------------------------------------------------------------------------------
      // Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());   // send a reply to the IP address and port that sent us the packet we received
      // Udp.write(ReplyBuffer);
      // Udp.endPacket();

      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());   // send a reply to the IP address and port that sent us the packet we received
      // Udp.write(packetBuffer);
      serializeJson(doc, Udp);
      Udp.endPacket();
    }

    // Clear all old data from packetBuffer
    for (unsigned int i = 0; i <= packetSize; i++) { packetBuffer[i] = '\0'; }
  }

  //delay(100);
}