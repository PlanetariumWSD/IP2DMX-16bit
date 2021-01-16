#ifndef Rows_h
#define Rows_h

#include <Arduino.h>
#include <DmxSimple.h>
#include <Ramp.h>

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

class Rows {
 public:
  Row getRow(byte rowNumber);

  Rows();

 private:
  Row rows[4] = {Row(0), Row(1), Row(2), Row(3)};
};

#endif