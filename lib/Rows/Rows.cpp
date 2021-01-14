#include <Arduino.h>
#include <Rows.h>

Row::Row(byte rowNumber) : rowNumber(rowNumber){};

void Row::update() {
  for (int i = 0; i <= 8; i++) updateSeat(i);
};

void Row::updateSeat(byte seatNumber) {
  updateChannelPair(rowNumber * 8 + seatNumber + 0, redRamp.update());
  updateChannelPair(rowNumber * 8 + seatNumber + 2, greenRamp.update());
  updateChannelPair(rowNumber * 8 + seatNumber + 4, blueRamp.update());
  updateChannelPair(rowNumber * 8 + seatNumber + 6, whiteRamp.update());
};

void Row::updateChannelPair(byte channel, unsigned short value) {
  byte courseValue = value / 255;
  byte fineValue = value % 255;
  DmxSimple.write(channel, fineValue);
  DmxSimple.write(++channel, courseValue);
};

Row rows[4] = {Row(0), Row(1), Row(2), Row(3)};