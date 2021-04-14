#include <Arduino.h>
#include <ArduinoJson.h>
#include <EthernetUdp.h>
#include <dmx.h>
#include <node.h>

// v0_2_0 Reciever in main scope (Can't pass arrays via Class Member Functions or any other scope really)----------------------
// in C++, Memberfunction arrays are local only and deleted on close.
// json are not native to C++ and temporary data structures only.
// Why not pass a stack pointer and using static json doc? Tried that, Fail.
//--------------------------------------------------------------------------------

Node nodes[5];

Dmx dmx;

bool receiver();

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};  // Physcial Hardware Address
IPAddress ip(1, 1, 1, 40);                          // Local Area Network Address Assigned
unsigned int localPort = 8888;                      // local port to listen on
EthernetUDP Udp;                                    // An EthernetUDP instance to let us send and receive packets over UDP

bool offlineTest = 1;
// This forces global allocation of json doc in memory to prevent it's destruction on a return.
DynamicJsonDocument jsondoc(354);  // Stored in dynamicly allocated RAM only! (ram usage optimized.)
//StaticJsonDocument<354> jsondoc;  // Allocates special non writeable space in heap RAM.

void setup() {
  Serial.begin(115200);
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
};

void loop() {
  if (receiver() == 1) {
    // DO COOL STUFF WITH NEW DATA

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
      Serial.print("NODE:");
      int theNode = target["node"];
      Serial.println(theNode);
      nodes[target["node"].as<uint8_t>() - 1].setTarget(target["val"], target["dur"], target["ramp"], target["loop"]);
    }
  }
  for (uint8_t i = 1; i <= 5; i++) {
    dmx.setBrightness(i, nodes[i - 1].getCurrentBrightness());
    Serial.print("Current brightness of node 2:");
    uint16_t node2Brightness = nodes[1].getCurrentBrightness();
    Serial.println(node2Brightness);
  }
};

bool receiver() {
  // if (offlineTest == 1) {
  //   offlineTest = 0;
  //   char testString[] = "{\"node\":1,\"val\":1000,\"dur\":3000,\"ramp\":16,\"loop\":2}";
  //   deserializeJson(jsondoc, testString);
  //   return 1;
  // } else {
  unsigned int packetSize = Udp.parsePacket();
  if (packetSize) {
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

    if (!error) {
      return 1;
    } else {
      return 0;
    }
  }
  //}
  return 0;
};