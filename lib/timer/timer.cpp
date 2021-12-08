#include <timer.h>

Timer::Timer() {
  reset();
};

void Timer::reset() {
  duration = -1;
};

void Timer::set(uint32_t newDuration) {
  start = millis();
  duration = newDuration;
};

bool Timer::ended() {
  uint32_t now = millis();
  uint32_t end = start + duration;

  // normal case
  if (end > start) return now > end || now < start;
  // overflow case
  if (end < start) return now > end && now < start;
  // duration 0 means it's always tripped, maximum means it's not set
  return duration == 0;
};