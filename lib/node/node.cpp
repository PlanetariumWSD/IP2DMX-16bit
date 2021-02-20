#include <node.h>

uint16_t Node::getCurrentValue() {
  return lightRamp.update();
};

void Node::setTarget(uint16_t val, uint32_t dur, uint8_t rampMode, uint8_t loopMode) {
  lightRamp.go(val, dur, static_cast<ramp_mode>(rampMode), static_cast<loop_mode>(loopMode));
}
