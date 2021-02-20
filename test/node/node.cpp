#include <Arduino.h>
#include <node.h>

Node node;

void setup() {
  Serial.begin(115200);
  while (!Serial) continue;

  node.setTarget(50000, 3000, 2, 4);
};

void loop() {
  Serial.println(node.getCurrentValue());
  delay(100);
  // ctrl+shift+L in Arduino IDE to see the beautiful graphs
};