#include <Arduino.h>
#include <dmx.h>
#include <node.h>

Node node;
Dmx dmx;

void setup() {
  Serial.begin(115200);
  while (!Serial) continue;

  node.loopMode = 1;
  node.setTarget(10000, 7000);
};

void loop() {
  uint16_t brightness = node.getCurrentBrightness();
  Serial.println(brightness);
  dmx.setBrightness(5, brightness);
  // ctrl+shift+L in Arduino IDE to see the beautiful graphs
  delay(5);
};