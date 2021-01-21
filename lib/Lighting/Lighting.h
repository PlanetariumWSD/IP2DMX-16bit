#ifndef Lighting_h
#define Lighting_h

#include <Arduino.h>
#include <DmxSimple.h>
#include <Ramp.h>

struct CoarseFineChannelPair {
  uint8_t coarse;
  uint8_t fine;
};

struct Channels {
  CoarseFineChannelPair red;
  CoarseFineChannelPair green;
  CoarseFineChannelPair blue;
  CoarseFineChannelPair white;
};

class Section {
 public:
  ramp redRamp;
  ramp greenRamp;
  ramp blueRamp;
  ramp whiteRamp;
  byte rowNumber;

  Section(Channels channels);

  void update();

 private:
  Channels channels;

  void updateCoarseFineChannelPair(CoarseFineChannelPair channelPair, uint16_t value);
};

class Lighting {
 public:
  Section getSection(uint8_t sectionNumber);

  Lighting();

  void update();

 private:
  Section sections[5] = {
      Section({{1, 2}, {3, 4}, {5, 6}, {7, 8}}),
      Section({{11, 12}, {13, 14}, {15, 16}, {17, 18}}),
      Section({{21, 22}, {23, 24}, {25, 26}, {27, 28}}),
      Section({{31, 32}, {33, 34}, {35, 36}, {37, 38}}),
      Section({{9, 10}, {19, 20}, {29, 30}, {39, 40}})};
};

#endif