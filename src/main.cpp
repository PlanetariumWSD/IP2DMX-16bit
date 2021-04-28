#include <Arduino.h>
#include <ArduinoJson.h>
#include <EthernetUdp.h>
#include <dmx.h>
#include <node.h>
#define NUMBER_OF_NODES 20

// v0_2_0 Reciever in main scope (Can't pass arrays via Class Member Functions or any other scope really)----------------------
// in C++, Memberfunction arrays are local only and deleted on close.
// json are not native to C++ and temporary data structures only.
// Why not pass a stack pointer and using static json doc? Tried that, Fail.
//--------------------------------------------------------------------------------

Node nodes[NUMBER_OF_NODES];
Dmx dmx;

bool receiver(DynamicJsonDocument &);

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};  // Physcial Hardware Address
IPAddress ip(1, 1, 1, 40);                          // Local Area Network Address Assigned
unsigned int localPort = 8888;                      // local port to listen on
EthernetUDP Udp;                                    // An EthernetUDP instance to let us send and receive packets over UDP

DynamicJsonDocument jsondoc(512);  // 508 max safe capacity of UDP packet.

void setup() {
  Serial.begin(115200);
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
};

void loop() {
  if (receiver(jsondoc) == 1) {
    serializeJsonPretty(jsondoc, Serial);
    Serial.println();
    JsonArray targets = jsondoc.as<JsonArray>();
    int fooNode = targets[0]["node"];
    Serial.println(fooNode);
    int fooVal = targets[0]["val"];
    Serial.println(fooVal);
    int fooDur = targets[0]["dur"];
    Serial.println(fooDur);

    for (JsonObject target : targets) {
      Serial.println(F("------"));
      Serial.print(F("NODE: "));
      Serial.println(target["node"].as<uint8_t>());
      nodes[target["node"].as<uint8_t>() - 1]
          .setTarget(target["val"], target["dur"], target["ramp"], target["loop"]);
    }
  }
  for (uint8_t i = 1; i <= NUMBER_OF_NODES; i++) {
    dmx.setBrightness(i, nodes[i - 1].getCurrentBrightness());
  }
};

bool receiver(DynamicJsonDocument &doc) {
  unsigned int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("PacketSize: ");
    Serial.println(packetSize);
    if (packetSize > 502) {
      Serial.println("Udp Packet too large");
      return 0;
    }
    DeserializationError error = deserializeJson(doc, Udp);

    switch (error.code()) {
      case DeserializationError::Ok:
        Serial.print(F("Deserialization succeeded"));
        break;
      case DeserializationError::InvalidInput:
        Serial.print(F("Invalid input!"));
        break;
      case DeserializationError::NoMemory:
        Serial.print(F("Not enough memory"));
        break;
      default:
        Serial.print(F("Deserialization failed"));
        break;
    }
    Serial.println();

    if (!error) {
      return 1;
    } else {
      return 0;
    }
  }
  return 0;
};

// if (packetSize > 502) {
//   Serial.println("Udp Packet too large");
//   return 0;
// }