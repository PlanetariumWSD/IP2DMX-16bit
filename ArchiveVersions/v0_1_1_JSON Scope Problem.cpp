#include <Arduino.h>
#include <ArduinoJson.h>
#include <EthernetUdp.h>
#include <dmx.h>
#include <node.h>

// v0_1_1 Reciever in main scope (Can't pass arrays via Class Member Functions or any other scope really)----------------------
// in C++, Memberfunction arrays are local only and deleted on close.
// json are not native to C++ and temporary data structures only.
// Why not passs a stack pointer and using dynamic json doc? Tried that but got "no memory error" from ArduinoJSON deserializer.
//--------------------------------------------------------------------------------

Node nodes[2];

Dmx dmx;

bool receiver();
void clearBuffer();

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};  // Physcial Hardware Address
IPAddress ip(1, 1, 1, 40);                          // Local Area Network Address Assigned
unsigned int localPort = 8888;                      // local port to listen on
EthernetUDP Udp;                                    // An EthernetUDP instance to let us send and receive packets over UDP

bool offlineTest = 1;
char packetBuffer[200] = "[{\"node\":1,\"val\":1000,\"dur\":3000,\"ramp\":16,\"loop\":2}]";
unsigned int packetSize = 70;
StaticJsonDocument<200> doc;

void setup() {
  Serial.begin(115200);
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
};

void loop() {
  if (receiver() == 1) {
    DeserializationError error = deserializeJson(doc, packetBuffer);
    if (!error) {
      JsonObjectConst targets = doc.as<JsonObject>();
      // DO STUFF WITH DATA
      int fooNode = targets["node"];
      Serial.println(fooNode);
      int fooVal = targets["val"];
      Serial.println(fooVal);
      int fooDur = targets["dur"];
      Serial.println(fooDur);
      //for (JsonObjectConst target : targets) {
      //  nodes[target["node"].as<uint8_t>() - 1].setTarget(target["val"], target["dur"], target["ramp"], target["loop"]);
      //}
      //for (uint8_t i = 1; i <= 20; i++) {
      //  dmx.setBrightness(i, nodes[i - 1].getCurrentBrightness());
    }
    clearBuffer();
  }
};

bool receiver() {
  if (offlineTest == 1) {
    offlineTest = 0;
    return 1;
  } else {
    packetSize = Udp.parsePacket();
    if (packetSize) {
      Udp.read(packetBuffer, packetSize);
      return 1;
    } else {
      return 0;
    }
  }
};

void clearBuffer() {
  // Clear all old data from packetBuffer
  for (unsigned int i = 0; i <= packetSize; i++) {
    packetBuffer[i] = '\0';
  }
};