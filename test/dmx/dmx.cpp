#include <Arduino.h>
#include <dmx.h>

Dmx dmx;

void setup() {
  dmx.setBrightness(6, 100);
  dmx.setBrightness(7, 100);
  dmx.setBrightness(8, 100);
};

void loop(){};