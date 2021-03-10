#include <dmx.h>

Dmx::Dmx() {
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  DmxSimple.usePin(4);
  DmxSimple.maxChannel(40);
};

void Dmx::setBrightness(uint8_t node, uint16_t brightness) {
  DmxSimple.write(node * 2 - 1, brightness / 256);
  DmxSimple.write(node * 2, brightness % 256);
}