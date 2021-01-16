#include <Arduino.h>
#include <ArduinoJson.h>
#include <KeywordParser.h>
#include <Receiver.h>
#include <Rows.h>

DynamicJsonDocument doc(15);

byte ip[4] = {1, 2, 3, 4};
byte mac[6] = {1, 2, 3, 4, 5, 6};
unsigned short port = 8080;
Receiver receiver(ip, mac, port);

Rows rows;

void setup() {}

void loop() {
  receiver.getJson(doc);
  if (doc != nullptr) {
    rows.getRow(doc["row"]).redRamp.go(doc["r"], doc["time"], parseRampMode(doc["rampMode"]), parseLoopMode(doc["loopMode"]));
    rows.getRow(doc["row"]).greenRamp.go(doc["g"], doc["time"], parseRampMode(doc["rampMode"]), parseLoopMode(doc["loopMode"]));
    rows.getRow(doc["row"]).blueRamp.go(doc["b"], doc["time"], parseRampMode(doc["rampMode"]), parseLoopMode(doc["loopMode"]));
    rows.getRow(doc["row"]).whiteRamp.go(doc["w"], doc["time"], parseRampMode(doc["rampMode"]), parseLoopMode(doc["loopMode"]));
  }

  for (byte i = 0; i <= 3; i++)
    rows.getRow(i).update();
}