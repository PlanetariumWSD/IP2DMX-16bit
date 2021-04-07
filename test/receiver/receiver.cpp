#include <Arduino.h>
#include <receiver.h>
#include <unity.h>

const uint8_t mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
const uint8_t ip[] = {1, 1, 1, 40};
const uint16_t port = 8888;

Receiver receiver(ip, mac, port);

void setup() {
  delay(2000);
  UNITY_BEGIN();

  Serial.begin(115200);
  while (!Serial) continue;

  JsonArray targets = receiver.getJson();

  Serial.print("Val: ");
  Serial.println((uint16_t)targets[0]["val"]);

  Serial.print("Dur: ");
  Serial.println((uint32_t)targets[0]["dur"]);

  Serial.print("Ramp: ");
  uint8_t ramp = targets[0]["ramp"];
  Serial.println(ramp);

  Serial.print("Loop: ");
  Serial.println((uint8_t)targets[0]["loop"]);

  UNITY_END();
};

void loop(){};