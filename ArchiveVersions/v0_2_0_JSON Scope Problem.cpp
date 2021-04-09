#include <Arduino.h>
#include <ArduinoJson.h>
#include <EthernetUdp.h>
#include <dmx.h>
#include <node.h>

// v0_2_0 Reciever in main scope (Can't pass arrays via Class Member Functions or any other scope really)----------------------
// in C++, Memberfunction arrays are local only and deleted on close.
// json are not native to C++ and temporary data structures only.
// Why not passs a stack pointer and using static json doc? Tried that, Fail.
//--------------------------------------------------------------------------------

Node nodes[20];

Dmx dmx;

bool receiver();

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};  // Physcial Hardware Address
IPAddress ip(1, 1, 1, 40);                          // Local Area Network Address Assigned
unsigned int localPort = 8888;                      // local port to listen on
EthernetUDP Udp;                                    // An EthernetUDP instance to let us send and receive packets over UDP

bool offlineTest = 1;
StaticJsonDocument<354> jsondoc;  // This forces global allocation of json doc in memory to prevent it's destruction on a return.

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
    JsonObject targets = jsondoc.as<JsonObject>();
    int fooNode = targets["node"];
    Serial.println(fooNode);
    int fooVal = targets["val"];
    Serial.println(fooVal);
    int fooDur = targets["dur"];
    Serial.println(fooDur);

    //for (JsonObject target : targets) {
    //  nodes[target["node"].as<uint8_t>() - 1].setTarget(target["val"], target["dur"], target["ramp"], target["loop"]);
    //}
    //for (uint8_t i = 1; i <= 20; i++) {
    //  dmx.setBrightness(i, nodes[i - 1].getCurrentBrightness());
    //}
  }
};

bool receiver() {
  if (offlineTest == 1) {
    offlineTest = 0;
    char testString[] = "{\"node\":1,\"val\":1000,\"dur\":3000,\"ramp\":16,\"loop\":2}";
    deserializeJson(jsondoc, testString);
    return 1;
  } else {
    unsigned int packetSize = Udp.parsePacket();
    if (packetSize) {
      DeserializationError error = deserializeJson(jsondoc, Udp);
      if (!error) {
        return 1;
      } else {
        return 0;
      }
    }
  }
  return 0;
};