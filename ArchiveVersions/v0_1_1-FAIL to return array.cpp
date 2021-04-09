#include <Arduino.h>
#include <ArduinoJson.h>
//#include <dmx.h>
//#include <node.h>
#include <receiver.h>

// v0_1_1 Pass Array back to main FAIL -------------------------------------------
//--------------------------------------------------------------------------------

const uint8_t ip[4] = {1, 1, 1, 40};
const uint8_t mac[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
const uint16_t port = 8888;
Receiver receiver(ip, mac, port);

//Node nodes[20];

//Dmx dmx;

void setup() {
  Serial.begin(115200);
};

void loop() {
  if (receiver.jsonIsAvailable()) {
    //JsonObject targets = receiver.getJson();
    //serializeJsonPretty(receiver.getJson(), Serial);
    // DO STUFF WITH DATA
    int fooNode = receiver.getJson()["node"];  // Attempt to use Memory Stack Pointer to "Desctructed" object.
    Serial.println(fooNode);
    //int fooVal = targets["val"];
    //Serial.println(fooVal);
    //int fooDur = targets["dur"];
    //Serial.println(fooDur);

    //for (JsonObjectConst target : targets) {
    //  nodes[target["node"].as<uint8_t>() - 1].setTarget(target["val"], target["dur"], target["ramp"], target["loop"]);
    //}
    delay(3000);
  }

  //for (uint8_t i = 1; i <= 20; i++) {
  //  dmx.setBrightness(i, nodes[i - 1].getCurrentBrightness());
  //}
};