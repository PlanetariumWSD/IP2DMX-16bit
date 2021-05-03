#include <Arduino.h>
#include <ArduinoJson.h>
#include <EthernetUdp.h>
//#include <Ramp.h>
//#include <dmx.h>
//#include <node.h>
//#define NUMBER_OF_NODES 1

// v0_2_2 Added logic to process json doc RAMP problem
//--------------------------------------------------------------------------------

//Node nodes[NUMBER_OF_NODES];
//Dmx dmx;

//rampUnsignedInt myRamp;

bool receiver();

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};  // Physcial Hardware Address
IPAddress ip(1, 1, 1, 40);                          // Local Area Network Address Assigned
unsigned int localPort = 8888;                      // local port to listen on
EthernetUDP Udp;                                    // An EthernetUDP instance to let us send and receive packets over UDP

void setup() {
  Serial.begin(115200);
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
};

void loop() {
  if (receiver() == 1) {
    StaticJsonDocument<512> jsondoc;  // 508 max safe capacity of UDP packet.
    DeserializationError error = deserializeJson(jsondoc, Udp);
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
    serializeJsonPretty(jsondoc, Serial);
    Serial.println();
    //JsonArray targets = jsondoc.as<JsonArray>();
    Serial.print(F("Val: "));
    int16_t FooV = jsondoc["val"];
    Serial.println(FooV);
    serializeJsonPretty(jsondoc, Serial);

    //Serial.println(targets["v"]);
    //Serial.println(targets["d"]);
    //myRamp.go(targets["v"], targets["d"]);

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

  //dmx.setBrightness(1, myRamp.update());
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