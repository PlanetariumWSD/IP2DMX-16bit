#include <Arduino.h>
#include <Timer.h>

Timer shortTimer;
Timer longTimer;

void setup() {}

void loop() {
  if (longTimer.wentOff()) {
    longTimer.set(6000);
  }

  if (shortTimer.wentOff()) {
    shortTimer.set(2000);
  }
}