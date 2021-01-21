#include <Arduino.h>
#include <ArduinoJson.h>
#include <KeywordParser.h>
#include <Lighting.h>
#include <Receiver.h>

DynamicJsonDocument doc(15);

uint8_t ip[4] = {1, 2, 3, 4};
uint8_t mac[6] = {1, 2, 3, 4, 5, 6};
uint16_t port = 8080;
Receiver receiver(ip, mac, port);

Lighting lighting;

uint16_t convertTo16Bit(uint8_t percent) {
  return percent / 100 * 65025;
}

void handleDoc(DynamicJsonDocument doc) {
  double duration = doc["duration"];
  duration *= 1000;
  auto rampMode = parseRampMode(doc["modes"]["ramp"]);
  auto loopMode = parseLoopMode(doc["modes"]["loop"]);
  for (uint8_t i = 0; i < sizeof(doc["sections"]) / sizeof(doc["sections"][0]); i++) {
    lighting.getSection(doc["sections"][i]).redRamp.go(convertTo16Bit(doc["values"]["r"]), duration, rampMode, loopMode);
    lighting.getSection(doc["sections"][i]).greenRamp.go(convertTo16Bit(doc["values"]["g"]), duration, rampMode, loopMode);
    lighting.getSection(doc["sections"][i]).blueRamp.go(convertTo16Bit(doc["values"]["b"]), duration, rampMode, loopMode);
    lighting.getSection(doc["sections"][i]).whiteRamp.go(convertTo16Bit(doc["values"]["w"]), duration, rampMode, loopMode);
  }
};

void setup(){};

void loop() {
  receiver.getJson(doc);
  if (doc != nullptr) handleDoc(doc);

  lighting.update();
};