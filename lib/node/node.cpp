#include <node.h>

uint16_t Node::getCurrentBrightness() {
  return lightRamp.update();
};

void Node::setTarget(uint16_t value, uint32_t duration, uint8_t rampMode, uint8_t loopMode) {
  lightRamp.go(value, duration, static_cast<ramp_mode>(rampMode), static_cast<loop_mode>(loopMode));
}
