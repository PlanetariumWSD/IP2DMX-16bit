#include <Arduino.h>
#include <DmxSimple.h>
#include <Ramp.h>

#ifndef Rows_h
#define Rows_h

class Row {
 public:
  ramp redRamp;
  ramp greenRamp;
  ramp blueRamp;
  ramp whiteRamp;
  byte rowNumber;

  Row(byte rowNumber);

  void update();

 private:
  void updateSeat(byte seatNumber);
  void updateChannelPair(byte channel, unsigned short value);
};

extern Row rows[4];

#endif