#ifndef Timer_h
#define Timer_h

#include <Arduino.h>

class Timer {
 private:
  uint32_t start;
  uint32_t duration;

 public:
  Timer();
  void set(uint32_t);
  void reset();
  bool ended();
};

#endif