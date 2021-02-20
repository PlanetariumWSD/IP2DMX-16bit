#ifndef Dmx_h
#define Dmx_h

#include <Arduino.h>
#include <DmxSimple.h>

class Dmx {
 public:
  Dmx();

  void setBrightness(uint8_t node, uint16_t brightness);
};

#endif