#include <Arduino.h>
#include <node.h>
#include <dmx.h>

Node node;
Dmx dmx;

void setup() {
  Serial.begin(115200);
  while (!Serial) continue;

  node.setTarget(1000, 3000, 10, 2);
};

void loop() {
  uint16_t brightness = node.getCurrentBrightness();
  Serial.println(brightness);
  dmx.setBrightness(4, brightness);
  // ctrl+shift+L in Arduino IDE to see the beautiful graphs
};