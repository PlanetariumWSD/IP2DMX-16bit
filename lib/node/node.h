#ifndef Node_h
#define Node_h

#include <Ramp.h>

class Node {
 private:
  rampUnsignedInt lightRamp;

 public:
  uint16_t getCurrentValue();
  void setTarget(uint16_t val, uint32_t dur, uint8_t rampMode, uint8_t loopMode);
};

#endif