#include <node.h>

Node::Node() {
  rampMode = 1;
  loopMode = 0;
}

uint16_t Node::getCurrentBrightness() {
  return lightRamp.update();
};

void Node::setTarget(uint16_t value, uint16_t duration) {
  lightRamp.go(value, (duration * 100), static_cast<ramp_mode>(rampMode), static_cast<loop_mode>(loopMode));
}

/** 
 * RAMP MODES
 * ---
 * NONE                = 0
 * LINEAR              = 1
 * QUADRATIC_IN        = 2
 * QUADRATIC_OUT       = 3
 * QUADRATIC_INOUT     = 4
 * CUBIC_IN            = 5
 * CUBIC_OUT           = 6
 * CUBIC_INOUT         = 7
 * QUARTIC_IN          = 8
 * QUARTIC_OUT         = 9
 * QUARTIC_INOUT       = 10
 * QUINTIC_IN          = 11
 * QUINTIC_OUT         = 12
 * QUINTIC_INOUT       = 13
 * SINUSOIDAL_IN       = 14
 * SINUSOIDAL_OUT      = 15
 * SINUSOIDAL_INOUT    = 16
 * EXPONENTIAL_IN      = 17
 * EXPONENTIAL_OUT     = 18
 * EXPONENTIAL_INOUT   = 19
 * CIRCULAR_IN         = 20
 * CIRCULAR_OUT        = 21
 * CIRCULAR_INOUT      = 22
 * ELASTIC_IN          = 23
 * ELASTIC_OUT         = 24
 * ELASTIC_INOUT       = 25
 * BACK_IN             = 26
 * BACK_OUT            = 27
 * BACK_INOUT          = 28
 * BOUNCE_IN           = 29
 * BOUNCE_OUT          = 30
 * BOUNCE_INOUT        = 31
 * 
 * LOOP MODES
 * ---
 * ONCEFORWARD         = 0
 * LOOPFORWARD         = 1
 * FORTHANDBACK        = 2
 * ONCEBACKWARD        = 3
 * LOOPBACKWARD        = 4
 * BACKANDFORTH        = 5
*/
