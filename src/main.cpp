#include <Arduino.h>
#include <dmx.h>
#include <node.h>
#include <receiver.h>

uint8_t ip[4] = {1, 2, 3, 4};
uint8_t mac[6] = {1, 2, 3, 4, 5, 6};
uint16_t port = 8080;
Receiver receiver(ip, mac, port);

Node nodes[20];

Dmx dmx;

void setup(){};

void loop() {
  if (receiver.jsonIsAvailable()) {
    JsonArray targets = receiver.getJson();

    for (JsonObject target : targets) {
      nodes[target["node"].as<uint8_t>() - 1].setTarget(target["val"], target["dur"], target["ramp"], target["loop"]);
    }
  }

  for (uint8_t i = 1; i <= 20; i++) {
    dmx.setBrightness(i, nodes[i - 1].getCurrentBrightness());
  }
};