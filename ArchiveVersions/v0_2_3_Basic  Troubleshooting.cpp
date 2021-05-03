#include <Arduino.h>
#include <ArduinoJson.h>
#include <EthernetUdp.h>
#include <Ramp.h>
#include <dmx.h>
//#include <node.h>
//#define NUMBER_OF_NODES 1

// v0_2_3 Basic Troubleshooting
//--------------------------------------------------------------------------------

//Node nodes[NUMBER_OF_NODES];
Dmx dmx;

rampUnsignedInt myRamp;

bool receiver();
char packetBuffer[200];
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};  // Physcial Hardware Address
IPAddress ip(1, 1, 1, 40);                          // Local Area Network Address Assigned
unsigned int localPort = 8888;                      // local port to listen on
EthernetUDP Udp;                                    // An EthernetUDP instance to let us send and receive packets over UDP

char json[] =
    "{\"n\":1,\"v\":10000,\"d\":1300}";
// "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

void setup() {
  Serial.begin(115200);
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
};

void loop() {
  unsigned int packetSize = Udp.parsePacket();
  if (packetSize) {
    Udp.read(packetBuffer, packetSize);  // read the packet into packetBuffer
    StaticJsonDocument<512> jsondoc;     // 508 max safe capacity of Udp packet.
    DeserializationError error = deserializeJson(jsondoc, packetBuffer);
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
    Serial.println(F("------"));
    //serializeJsonPretty(jsondoc, Serial);
    serializeJson(jsondoc, Serial);
    Serial.println();
    //JsonArray targets = jsondoc.as<JsonArray>();
    uint32_t value = jsondoc["v"];
    Serial.print(F("v: "));
    Serial.println(value);
    uint32_t dur = jsondoc["d"];
    //uint32_t dur = jsondoc["d"].as<uint32_t>();
    Serial.print(F("d: "));
    Serial.println(dur);
    int8_t rampKind = jsondoc["r"];
    Serial.print(F("r: "));
    Serial.println(rampKind);
    int8_t loopKind = jsondoc["l"];
    Serial.print(F("l: "));
    Serial.println(loopKind);
    serializeJsonPretty(jsondoc, Serial);

    myRamp.go(value, dur, rampKind, loopKind);

    // ------------------------------RETURN
    //    JsonArray targets = jsondoc.as<JsonArray>();
    //    for (JsonObject target : targets) {
    //      uint8_t nodeNumber = target["n"].as<uint8_t>();
    //
    //      Serial.print(F("NODE: "));
    //      Serial.println(nodeNumber);
    //      if (!target["r"].isNull()) nodes[nodeNumber - 1].rampMode = target["r"];
    //      if (!target["l"].isNull()) nodes[nodeNumber - 1].loopMode = target["l"];
    //      nodes[nodeNumber - 1].setTarget(target["v"], target["d"]);
    //    }
    //  }
    //  for (uint8_t i = 1; i <= NUMBER_OF_NODES; i++) {
    //    dmx.setBrightness(i, nodes[i - 1].getCurrentBrightness());
    //    // uint16_t brightness = nodes[1].getCurrentBrightness();
    //    // Serial.println(brightness);
  };

  dmx.setBrightness(1, myRamp.update());
  //Serial.println(myRamp.update());
};

bool receiver() {
  unsigned int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("PacketSize: ");
    Serial.println(packetSize);
    if (packetSize > 502) {
      Serial.println("Udp Packet too large");
      return 0;
    } else
      return 1;
  } else
    return 0;
};