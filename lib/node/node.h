#ifndef Node_h
#define Node_h

#include <Ramp.h>

class Node {
 private:
  rampUnsignedInt lightRamp;

 public:
  uint8_t rampMode;
  uint8_t loopMode;
  uint16_t getCurrentBrightness();
  void setTarget(uint16_t value, uint32_t duration);
  Node();
};

#endif