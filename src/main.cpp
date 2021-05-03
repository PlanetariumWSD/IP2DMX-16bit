#include <Arduino.h>
#include <ArduinoJson.h>
#include <EthernetUdp.h>
#include <dmx.h>
#include <node.h>
#define NUMBER_OF_NODES 20

// v0_3_1 Parsing light CMD Logic complete (Needs device "node:0" CMDs)
//--------------------------------------------------------------------------------

Node nodes[NUMBER_OF_NODES];
Dmx dmx;

bool receiver(DynamicJsonDocument &);

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};  // Physcial Hardware Address
IPAddress ip(1, 1, 1, 40);                          // Local Area Network Address Assigned
unsigned int localPort = 8888;                      // local port to listen on
EthernetUDP Udp;                                    // An EthernetUDP instance to let us send and receive packets over UDP

DynamicJsonDocument jsondoc(768);  // 508 max safe capacity of UDP packet. (Assistant says 657 min, 768)

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
    for (JsonObject target : targets) {
      Serial.println(F("------"));
      Serial.print(F("NODE: "));
      uint8_t nodeNumber = target["n"].as<uint8_t>();
      Serial.println(nodeNumber);
      if (!target["r"].isNull()) nodes[nodeNumber - 1].rampMode = target["r"];
      if (!target["l"].isNull()) nodes[nodeNumber - 1].loopMode = target["l"];
      if (!target["v"].isNull()) {
        if (!target["d"].isNull())
          nodes[nodeNumber - 1].setTarget(target["v"], target["d"]);
        else
          nodes[nodeNumber - 1].setTarget(target["v"], 0);
      }
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