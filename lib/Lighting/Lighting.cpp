#include <Arduino.h>
#include <Lighting.h>

Section::Section(Channels channels) : channels(channels){};

void Section::update() {
  updateCoarseFineChannelPair(channels.red, redRamp.update());
  updateCoarseFineChannelPair(channels.green, greenRamp.update());
  updateCoarseFineChannelPair(channels.blue, blueRamp.update());
  updateCoarseFineChannelPair(channels.white, whiteRamp.update());
};

void Section::updateCoarseFineChannelPair(CoarseFineChannelPair channelPair, uint16_t value) {
  DmxSimple.write(channelPair.fine, value % 255);
  DmxSimple.write(channelPair.coarse, value / 255);
};

Lighting::Lighting() {
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  DmxSimple.usePin(4);
  DmxSimple.maxChannel(40);
};

Section Lighting::getSection(uint8_t sectionNumber) {
  return sections[sectionNumber - 1];
}

void Lighting::update() {
  for (uint8_t i = 1; i <= 5; i++)
    getSection(i).update();
}